/**
 * @file MolSim.cpp
 * @brief Main entry point for the Molecular Dynamics simulation.
 */

#include <spdlog/spdlog.h>

#include <exception>

#include "ParticleContainer.h"
#include "Simulation/SimulationFactory.h"
#include "inputReader/Arguments.h"
#include "inputReader/InputReader.h"
#include "inputReader/SimulationConfig.h"
#include "inputReader/YamlInputReader.h"
#include "logging.hpp"

/**
 * @brief Main entry point of the molecular dynamics simulation.
 *
 * The program expects a single argument: the path to a YAML configuration file.
 * Use -h or --help to display usage information.
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return EXIT_SUCCESS (0) on successful completion, EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
  // Initialize logging (console + simulation.log).
  logging::init_logging();

  ParticleContainer particles;

  SPDLOG_INFO("MolSim starting...");

  Arguments args{};
  inputReader::parseArguments(argc, argv, args);

  SPDLOG_INFO("Reading YAML configuration from '{}'", args.inputFile);

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

  SPDLOG_INFO("Creating simulation (sim_type='{}')", static_cast<int>(cfg.sim_type));

  auto simulation = SimulationFactory::createSimulation(cfg, particles);

  SPDLOG_INFO("Starting simulation run...");
  simulation->runSimulation();
  SPDLOG_INFO("Simulation finished. Output written. Terminating.");

  return EXIT_SUCCESS;
}
