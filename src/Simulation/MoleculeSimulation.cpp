/**
 * @file MoleculeSimulation.cpp
 * @brief Implementation of the molecular dynamics simulation using Lennard-Jones.
 */

#include "MoleculeSimulation.h"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "Container/ContainerType.h"
#include "Container/LinkedCellContainer.h"
#include "ForceCalculation/LennardJones.h"
#include "inputReader/CheckpointReader.h"
#include "Generator/CuboidGenerator.h"
#include "Generator/DiscGenerator.h"
#include "Generator/ParticleGenerator.h"
#include "outputWriter/WriterFactory.h"

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
  for (const auto &d : cfg_.discs) {
    DiscGenerator::generateDisc(particles_, d.center, d.radiusCells, d.hDisc, d.mass, d.baseVelocity, d.typeDisc);
  }
  if (!cfg_.discs.empty()) {
    SPDLOG_INFO("Added {} disc(s); particle count is now {}.", cfg_.discs.size(), particles_.size());
  }

  // Lennard-Jones force setup
  LennardJones lj;
  lj.setEpsilon(cfg_.lj_epsilon);
  lj.setSigma(cfg_.lj_sigma);
  lj.setTypeParameters(cfg_.lj_types);
  lj.setGravity(cfg_.gravity);

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
    LennardJones::calculateX(particles_, cfg_.delta_t);
    if (cfg_.containerType == ContainerType::Cell) {
      static_cast<LinkedCellContainer *>(&particles_)->rebuild();
    }
    lj.calculateF(particles_);
    LennardJones::calculateV(particles_, cfg_.delta_t);

    iteration++;

    // Write output every cfg_.write_frequency
    if (iteration % cfg_.write_frequency == 0) {
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
  std::filesystem::create_directories("output");

  // Output file name from Outputformat
  std::string out_name = (format == OutputFormat::Checkpoint) ? "output/checkpoint" : "output/outputVTK";

  const auto writer = WriterFactory::createWriter(format);

  SPDLOG_DEBUG("Plotting {} particles at iteration {} to '{}'.", particles.size(), iteration, out_name);

  writer->plotParticles(particles, out_name, iteration);
}
