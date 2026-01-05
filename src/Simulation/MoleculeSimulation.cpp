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
#include "Generator/CuboidGenerator.h"
#include "Generator/DiscGenerator.h"
#include "Generator/ParticleGenerator.h"
#include "outputWriter/WriterFactory.h"

MoleculeSimulation::MoleculeSimulation(const SimulationConfig &cfg, Container &particles)
    : cfg_(cfg), particles_(particles) {}

void MoleculeSimulation::runSimulation() {
  SPDLOG_INFO("Setting up molecule simulation from YAML configuration...");

  // Generate particles from cuboids defined in cfg_.cuboids
  SPDLOG_INFO("Generating particles from {} cuboid(s)...", cfg_.cuboids.size());

  for (const auto &c : cfg_.cuboids) {
    CuboidGenerator::generateCuboid(particles_, c.origin, c.numPerDim, cfg_.domainSize, c.h, c.mass, c.baseVelocity,
                                    c.brownianMean, c.type);
  }

  // Generate particles from discs defined in cfg_.discs
  SPDLOG_INFO("Generating particles from {} disc(s)...", cfg_.discs.size());
  for (const auto &d : cfg_.discs) {
    DiscGenerator::generateDisc(particles_, d.center, d.radiusCells, d.hDisc, d.mass, d.baseVelocity, d.typeDisc);
  }

  SPDLOG_INFO("Generated {} particles", particles_.size());

  // Initialize thermostat
  std::unique_ptr<Thermostat> thermostat = nullptr;
  if (cfg_.thermostat.enable_thermostat) {
    thermostat = std::make_unique<Thermostat>(
     cfg_.thermostat.t_init,
     cfg_.thermostat.dimensions,
     cfg_.thermostat.n_thermostat,
     cfg_.thermostat.t_target,
     cfg_.thermostat.delta_t,
     cfg_.thermostat.brownian_motion
     );
  }

  // Lennard-Jones force setup
  LennardJones lj;
  lj.setEpsilon(5);
  lj.setSigma(1);

  // Initial force evaluation
  lj.calculateF(particles_);
  SPDLOG_DEBUG("Initial Lennard-Jones forces computed (epsilon=5, sigma=1).");

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

    // Thermostat application
    if (thermostat) {
      thermostat->apply(particles_, iteration);
    }

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

  SPDLOG_INFO("Molecule simulation completed after {} iterations (final t = {:.6g}).", iteration, current_time);
}

void MoleculeSimulation::plotParticles(Container &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");

  // Output file name from Outputformat
  std::string out_name = "output/outputVTK";

  const auto writer = WriterFactory::createWriter(format);

  SPDLOG_DEBUG("Plotting {} particles at iteration {} to '{}'.", particles.size(), iteration, out_name);

  writer->plotParticles(particles, out_name, iteration);
}