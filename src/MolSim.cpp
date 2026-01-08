/**
 * @file MolSim.cpp
 * @brief Main entry point for the Molecular Dynamics simulation.
 *
 * This file contains the application-level setup and execution logic.
 * In addition to initializing and running the simulation, it measures
 * the wall-clock runtime of the main simulation loop and reports a
 * derived performance metric (molecule updates per second).
 */

#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>
#include <exception>
#include <iostream>

#include "Container/ContainerFactory.h"
#include "Container/ParticleContainer.h"
#include "Simulation/SimulationFactory.h"
#include "inputReader/Arguments.h"
#include "inputReader/InputReader.h"
#include "inputReader/SimulationConfig.h"
#include "inputReader/YamlInputReader.h"
#include "utils/logging.hpp"

/**
 * @brief Main entry point of the molecular dynamics simulation.
 *
 * The program expects a single argument: the path to a YAML configuration file.
 * Use -h or --help to display usage information.
 * The function measures the wall-clock runtime of the main simulation loop
 * and reports a performance metric in terms of molecule updates per second.
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return EXIT_SUCCESS (0) on successful completion, EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
  // Initialize logging (console + simulation.log).
  logging::init_logging();

  SPDLOG_INFO("MolSim starting...");

  Arguments args{};
  inputReader::parseArguments(argc, argv, args);

  SPDLOG_INFO("Reading YAML configuration from '{}'", args.inputFile);

  SPDLOG_INFO("Creating simulation (input='{}')", args.inputFile);
  // create and populate the simulation configuration struct
  SimulationConfig cfg;
  try {
    YamlInputReader yamlReader(args.inputFile);
    cfg = yamlReader.parse();
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Failed to read YAML configuration: {}", e.what());
    std::cerr << "Error: " << e.what() << "\n\n";
    inputReader::printUsage();
    return EXIT_FAILURE;
  }

  auto container = ContainerFactory::createContainer(cfg);
  auto &particles = *container;

  SPDLOG_INFO("Creating simulation (sim_type='{}')", static_cast<int>(cfg.sim_type));

  auto simulation = SimulationFactory::createSimulation(cfg, particles);

  SPDLOG_INFO("Starting simulation run...");

  // Timing of main simulation loop
  const auto start_time = std::chrono::high_resolution_clock::now();

  simulation->runSimulation();

  const auto end_time = std::chrono::high_resolution_clock::now();

  SPDLOG_INFO("Simulation finished. Output written. Terminating.");

  // Elapsed wall-clock time in seconds
  const std::chrono::duration<double> elapsed = end_time - start_time;
  const double runtime_seconds = elapsed.count();

  // Compute the intended number of integration steps from the configuration.
  // We floor to next smallest integer because (t_end - t_start) / delta_t may be
  // slightly below/above an integer due to floating-point representation.
  // Note that the division itself may still be affected by floating-point representation error.

  const auto iterations = static_cast<std::size_t>(std::floor((cfg.t_end - cfg.t_start) / cfg.delta_t));

  // Number of physical particles only.
  // Ghost particles used for boundary handling are excluded, as they do not
  // represent independent molecules, but temporary copies that are not
  // time-integrated.
  const std::size_t num_particles = particles.size();

  // Molecule updates per second: (particles × iterations) / runtime
  const double molecule_updates_per_second =
      (static_cast<double>(num_particles) * static_cast<double>(iterations)) / runtime_seconds;

  // Print results to command line. Independent of logging flag passed during compilation
  std::cout << "Runtime [s]: " << runtime_seconds << '\n';
  std::cout << "Molecule updates per second: " << molecule_updates_per_second << '\n';


  return EXIT_SUCCESS;
}