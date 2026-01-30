/**
 * @file MembraneSimulation.cpp
 * @brief Membrane simulation using harmonic springs and repulsive Lennard-Jones.
 */

#include "MembraneSimulation.h"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "Container/ContainerType.h"
#include "ForceCalculation/ForceCalculation.h"
#include "Generator/MembraneGenerator.h"
#include "outputWriter/WriterFactory.h"

MembraneSimulation::MembraneSimulation(const SimulationConfig &cfg, Container &particles)
    : cfg_(cfg),
      particles_(particles),
      membrane_force_(cfg.membrane.epsilon, cfg.membrane.sigma, cfg.membrane.k, cfg.membrane.r0,
                      cfg.membrane.pull_force, cfg.membrane.pull_until, cfg.gravity) {}

void MembraneSimulation::runSimulation() {
  SPDLOG_INFO("Setting up membrane simulation...");

  if (cfg_.containerType == ContainerType::Cell) {
    static_cast<LinkedCellContainer *>(&particles_)->setBoundaryConditions(cfg_.boundaryConditions);
  }

  grid_ = MembraneGenerator::generate(particles_, cfg_.membrane);

  if (cfg_.containerType == ContainerType::Cell) {
    static_cast<LinkedCellContainer *>(&particles_)->rebuild();
  }

  membrane_force_.setPullTargets(collectPullTargets());
  membrane_force_.setCurrentTime(cfg_.t_start);
  membrane_force_.calculateF(particles_);

  double current_time = cfg_.t_start;
  int iteration = 0;

  SPDLOG_INFO("Starting membrane simulation: t_start={}, t_end={}, delta_t={}, output every {} steps.", cfg_.t_start,
              cfg_.t_end, cfg_.delta_t, cfg_.write_frequency);

  while (current_time < cfg_.t_end) {
    ForceCalculation::calculateX(particles_, cfg_.delta_t);
    if (cfg_.containerType == ContainerType::Cell) {
      static_cast<LinkedCellContainer *>(&particles_)->rebuild();
    }

    membrane_force_.setCurrentTime(current_time);
    membrane_force_.calculateF(particles_);
    ForceCalculation::calculateV(particles_, cfg_.delta_t);

    iteration++;

    if (iteration % cfg_.write_frequency == 0) {
      if (cfg_.containerType == ContainerType::Cell) {
        static_cast<LinkedCellContainer *>(&particles_)->deleteHaloCells();
      }
      SPDLOG_INFO("Writing output at iteration {} (t = {:.6g}).", iteration, current_time);
      plotParticles(particles_, iteration, cfg_.output_format);
    }

    current_time += cfg_.delta_t;
  }

  if (cfg_.output_format == OutputFormat::Checkpoint && iteration % cfg_.write_frequency != 0) {
    if (cfg_.containerType == ContainerType::Cell) {
      static_cast<LinkedCellContainer *>(&particles_)->deleteHaloCells();
    }
    SPDLOG_INFO("Writing final checkpoint at iteration {} (t = {:.6g}).", iteration, current_time);
    plotParticles(particles_, iteration, cfg_.output_format);
  }

  SPDLOG_INFO("Membrane simulation completed after {} iterations (final t = {:.6g}).", iteration, current_time);
}

void MembraneSimulation::plotParticles(Container &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");

  std::string out_name = (format == OutputFormat::Checkpoint) ? "output/checkpoint" : "output/outputVTK";

  const auto writer = WriterFactory::createWriter(format);

  SPDLOG_DEBUG("Plotting {} particles at iteration {} to '{}'.", particles.size(), iteration, out_name);

  writer->plotParticles(particles, out_name, iteration);
}

std::vector<Particle *> MembraneSimulation::collectPullTargets() const {
  std::vector<Particle *> targets;
  targets.reserve(cfg_.membrane.pull_indices.size());
  for (const auto &idx : cfg_.membrane.pull_indices) {
    const int i = idx[0];
    const int j = idx[1];
    auto *p = MembraneGenerator::particleAt(grid_, cfg_.membrane.n, i, j, 0);
    if (p) {
      targets.push_back(p);
    } else {
      SPDLOG_WARN("Pull target ({}, {}) is out of bounds for membrane size ({}, {}).", i, j, cfg_.membrane.n[0],
                  cfg_.membrane.n[1]);
    }
  }
  return targets;
}
