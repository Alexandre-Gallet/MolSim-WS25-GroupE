/**
 * @file PlanetSimulation.cpp
 * @brief Implementation of gravitational planet simulation.
 */

#include "PlanetSimulation.h"

#include <filesystem>
#include <spdlog/spdlog.h>

#include "../ForceCalculation/StormerVerlet.h"
#include "../outputWriter/WriterFactory.h"

PlanetSimulation::PlanetSimulation(const SimulationConfig &cfg, ParticleContainer &particles)
    : cfg_(cfg), particles_(particles) {}

void PlanetSimulation::runSimulation() {
  // Planet simulation initial condition setup

  if (particles_.empty()) {
    SPDLOG_WARN("PlanetSimulation: No initial particles present! Check YAML configuration.");
  }

  double current_time = cfg_.t_start;
  int iteration = 0;

  SPDLOG_INFO("Starting planet simulation: t_start={}, t_end={}, delta_t={}, output every {} steps.",
               cfg_.t_start, cfg_.t_end, cfg_.delta_t, cfg_.write_frequency);

  // Time integration loop (Störmer–Verlet)

  StormerVerlet verlet;

  while (current_time < cfg_.t_end) {
    // calculate new positions
    StormerVerlet::calculateX(particles_, cfg_.delta_t);

    // calculate new forces
    verlet.calculateF(particles_);

    // calculate new velocities
    StormerVerlet::calculateV(particles_, cfg_.delta_t);

    iteration++;

    if (iteration % cfg_.write_frequency == 0) {
      SPDLOG_INFO("Writing output at iteration {} (t = {}).", iteration, current_time);
      plotParticles(particles_, iteration, cfg_.output_format);
    }

    current_time += cfg_.delta_t;
  }

  SPDLOG_INFO("Planet simulation completed after {} iterations (final t = {:.6g}).",
               iteration, current_time);
}

void PlanetSimulation::plotParticles(ParticleContainer &particles, int iteration, OutputFormat format) {
  std::filesystem::create_directories("output");

  std::string out_name = strcat("output/", OutputFormatToString(format));

  const auto writer = WriterFactory::createWriter(format);

  writer->plotParticles(particles, out_name, iteration);
}
