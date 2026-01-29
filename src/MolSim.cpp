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

  // Print results to command line. Independent of logging flag passed during compilation
  std::cout << "Runtime [s]: " << runtime_seconds << '\n';
  std::cout << std::flush;

  return EXIT_SUCCESS;
}

/*
- TODO(Task 2): Parallelization still needs to be finished

- TODO: slurm scripts for parallel section of the cluster

- TODO: Clearer names for input files

- TODO: warnings mostly in testing files during building should be fixed

- TODO: slides, which we should do using the online powerpoint version so we can all work on them in parallel

- TODO(Task 3): Full-length 3D Rayleigh–Taylor simulation. (Which one is the the correct input file) This is the one that we might have to run with smaller end_time due to timing constraints

- TODO(output files): Which input files are the ones that need to be run for your visualizations.

- TODO(Profiling): Some recent optimizations to the innerloops were done without profiling. This is important so you can argue evidence based approach

- TODO(Meeting Friday Morning): I have a room in the TUM bib from 08:00 to 10:00
*/