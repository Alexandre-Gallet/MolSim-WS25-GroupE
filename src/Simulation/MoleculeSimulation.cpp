/**
 * @file MoleculeSimulation.cpp
 * @brief Implementation of the molecular dynamics simulation using Lennard-Jones.
 */

#include "MoleculeSimulation.h"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "ForceCalculation/LennardJones.h"
#include "ParticleGenerator.h"
#include "outputWriter/WriterFactory.h"

MoleculeSimulation::MoleculeSimulation(const SimulationConfig &cfg, ParticleContainer &particles)
    : cfg_(cfg), particles_(particles) {}

void MoleculeSimulation::runSimulation() {
  SPDLOG_INFO("Setting up molecule simulation from YAML configuration...");

  // Generate particles from cuboids defined in cfg_.cuboids
  SPDLOG_INFO("Generating particles from {} cuboid(s)...", cfg_.cuboids.size());

  for (const auto &c : cfg_.cuboids) {
    ParticleGenerator::generateCuboid(particles_, c.origin, c.numPerDim, c.h, c.mass, c.baseVelocity, c.brownianMean,
                                      c.type);
  }

  SPDLOG_INFO("Generated {} particles from cuboids.", particles_.size());

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

  while (current_time < cfg_.t_end) {
    // integrate positions (x), then recompute forces, then velocities (v)
    LennardJones::calculateX(particles_, cfg_.delta_t);
    lj.calculateF(particles_);
    LennardJones::calculateV(particles_, cfg_.delta_t);

    iteration++;

    // Write output every cfg_.write_frequency
    if (iteration % cfg_.write_frequency == 0) {
      SPDLOG_INFO("Writing output at iteration {} (t = {:.6g}).", iteration, current_time);
      plotParticles(particles_, iteration, cfg_.output_format);
    }

    SPDLOG_DEBUG("Iteration {} finished (t = {}).", iteration, current_time);

    current_time += cfg_.delta_t;
  }

  SPDLOG_INFO("Molecule simulation completed after {} iterations (final t = {:.6g}).", iteration, current_time);
}

void MoleculeSimulation::plotParticles(ParticleContainer &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");

  // Output file name from Outputformat
  std::string out_name = "output/outputVTK";

  const auto writer = WriterFactory::createWriter(format);

  SPDLOG_DEBUG("Plotting {} particles at iteration {} to '{}'.", particles.size(), iteration, out_name);

  writer->plotParticles(particles, out_name, iteration);
}