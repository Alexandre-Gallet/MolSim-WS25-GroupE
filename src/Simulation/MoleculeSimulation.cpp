/**
 * @file MoleculeSimulation.cpp
 * @brief Implementation of the molecular dynamics simulation using Lennard-Jones.
 */

#include "MoleculeSimulation.h"

#include <spdlog/spdlog.h>
#include <utils/NanoScaleThermostat.h>

#include <filesystem>

#include "Container/ContainerType.h"
#include "Container/LinkedCellContainer.h"
#include "ForceCalculation/LennardJones.h"
#include "Generator/CuboidGenerator.h"
#include "Generator/DiscGenerator.h"
#include "Generator/ParticleGenerator.h"
#include "inputReader/CheckpointReader.h"
#include "outputWriter/WriterFactory.h"
#include "utils/ParticleProfiling.h"

MoleculeSimulation::MoleculeSimulation(const SimulationConfig &cfg, Container &particles)
    : cfg_(cfg), particles_(particles) {}

void MoleculeSimulation::runSimulation() {
  SPDLOG_INFO("Setting up molecule simulation from YAML configuration...");

  const bool restarted = cfg_.restart_from_checkpoint;
  if (restarted) {
    SPDLOG_INFO("Loading particles from checkpoint '{}'.", cfg_.checkpoint_file);
    inputReader::CheckpointReader::readFile(particles_, cfg_.checkpoint_file);
    SPDLOG_INFO("Loaded {} particles from checkpoint.", particles_.size());
  } else {
    // Generate particles from cuboids defined in cfg_.cuboids
    SPDLOG_INFO("Generating particles from {} cuboid(s)...", cfg_.cuboids.size());

    for (const auto &c : cfg_.cuboids) {
      CuboidGenerator::generateCuboid(particles_, c.origin, c.numPerDim, cfg_.domainSize, c.h, c.mass, c.baseVelocity,
                                      c.brownianMean, c.type);
    }
    SPDLOG_INFO("Generated {} particles from cuboids.", particles_.size());
  }

  // Discs can also be added on top of a checkpoint (used for the falling drop).
  // Generate particles from discs defined in cfg_.discs
  SPDLOG_INFO("Generating particles from {} disc(s)...", cfg_.discs.size());
  for (const auto &d : cfg_.discs) {
    DiscGenerator::generateDisc(particles_, d.center, d.radiusCells, d.hDisc, d.mass, d.baseVelocity, d.typeDisc);
  }
  if (!cfg_.discs.empty()) {
    SPDLOG_INFO("Added {} disc(s); particle count is now {}.", cfg_.discs.size(), particles_.size());
  }

  // Initialize thermostat
  std::unique_ptr<Thermostat> thermostat = nullptr;
  if (cfg_.thermostat.enable_thermostat) {
    thermostat = std::make_unique<Thermostat>(cfg_.thermostat.t_init, cfg_.thermostat.dimensions,
                                              cfg_.thermostat.n_thermostat, cfg_.thermostat.t_target,
                                              cfg_.thermostat.delta_t, cfg_.thermostat.brownian_motion);
  }
  // Initialize thermostat for the nano scale flow simulation
  std::unique_ptr<NanoScaleThermostat> ns_thermo = nullptr;
  if (cfg_.ns_thermostat.enable_thermostat) {
    ns_thermo = std::make_unique<NanoScaleThermostat>(cfg_.ns_thermostat.t_init, cfg_.ns_thermostat.dimensions,
                                                      cfg_.ns_thermostat.n_thermostat, cfg_.ns_thermostat.t_target,
                                                      cfg_.ns_thermostat.delta_t, cfg_.ns_thermostat.brownian_motion);
  }

  // Lennard-Jones force setup
  LennardJones lj;
  lj.setEpsilon(cfg_.lj_epsilon);
  lj.setSigma(cfg_.lj_sigma);
  lj.setTypeParameters(cfg_.lj_types);
  lj.setGravity(cfg_.gravity);
  lj.setEnableOmpForces(cfg_.parallel_strategy == ParallelStrategy::Force);

  // Initial force evaluation
  lj.calculateF(particles_);
  SPDLOG_DEBUG("Initial Lennard-Jones forces computed (epsilon={}, sigma={}).", cfg_.lj_epsilon, cfg_.lj_sigma);

  // Time integration loop
  double current_time = cfg_.t_start;
  int iteration = 0;

  SPDLOG_INFO("Starting molecule simulation: t_start={}, t_end={}, delta_t={}, output every {} steps.", cfg_.t_start,
              cfg_.t_end, cfg_.delta_t, cfg_.write_frequency);

  if (cfg_.containerType == ContainerType::Cell) {
    static_cast<LinkedCellContainer *>(&particles_)->setBoundaryConditions(cfg_.boundaryConditions);
  }

  while (current_time < cfg_.t_end) {
    // integrate positions (x), then recompute forces, then velocities (v)
    if (cfg_.parallel_strategy == ParallelStrategy::Integrate) {
      ForceCalculation::calculateXParallel(particles_, cfg_.delta_t);
    } else {
      LennardJones::calculateX(particles_, cfg_.delta_t);
    }
    if (cfg_.containerType == ContainerType::Cell) {
      static_cast<LinkedCellContainer *>(&particles_)->rebuild();
    }
    lj.calculateF(particles_);
    if (cfg_.parallel_strategy == ParallelStrategy::Integrate) {
      ForceCalculation::calculateVParallel(particles_, cfg_.delta_t);
    } else {
      LennardJones::calculateV(particles_, cfg_.delta_t);
    }

    // Thermostat application
    if (thermostat) {
      thermostat->apply(particles_, iteration);
    }
    if (ns_thermo) {
      ns_thermo->apply(particles_, iteration);
    }

    iteration++;

    // Write the particle profiling only for the nano scale simulation
    if (ns_thermo) {
      if (iteration % 10000 == 0) {
        ParticleProfiling::computeProfiling(particles_, cfg_.domainSize[0], cfg_.domainSize[1], cfg_.domainSize[2], 20,
                                            iteration);
      }
    }

    // Write output every on every modulo write_frequency == 0 and if OutputFormat
    // is not set to None to avoid the entire loop during benchmarking and not have to resort to setting
    // write_frequency >> 100 billion
    if (cfg_.output_format != OutputFormat::NONE && iteration % cfg_.write_frequency == 0) {
      if (cfg_.containerType == ContainerType::Cell) {
        static_cast<LinkedCellContainer *>(&particles_)->deleteHaloCells();
      }
      SPDLOG_INFO("Writing output at iteration {} (t = {:.6g}).", iteration, current_time);
      plotParticles(particles_, iteration, cfg_.output_format);
    }

    SPDLOG_DEBUG("Iteration {} finished (t = {}).", iteration, current_time);

    current_time += cfg_.delta_t;
  }

  // Ensure a final checkpoint is written when requested, even if the loop did not land on write_frequency.
  // Won't affect benchmarking if outputFormat None is chose
  if (cfg_.output_format == OutputFormat::Checkpoint && iteration % cfg_.write_frequency != 0) {
    if (cfg_.containerType == ContainerType::Cell) {
      static_cast<LinkedCellContainer *>(&particles_)->deleteHaloCells();
    }
    SPDLOG_INFO("Writing final checkpoint at iteration {} (t = {:.6g}).", iteration, current_time);
    plotParticles(particles_, iteration, cfg_.output_format);
  }

  SPDLOG_INFO("Molecule simulation completed after {} iterations (final t = {:.6g}).", iteration, current_time);
}

void MoleculeSimulation::plotParticles(Container &particles, int iteration, OutputFormat format) {
  // double check that benchmarking/profiling never uses plotParticles
  if (format == OutputFormat::NONE) {
    return;
  }

  std::filesystem::create_directories("output");

  // Output file name from Outputformat
  std::string out_name = (format == OutputFormat::Checkpoint) ? "output/checkpoint" : "output/outputVTK";

  const auto writer = WriterFactory::createWriter(format);

  SPDLOG_DEBUG("Plotting {} particles at iteration {} to '{}'.", particles.size(), iteration, out_name);

  writer->plotParticles(particles, out_name, iteration);
}