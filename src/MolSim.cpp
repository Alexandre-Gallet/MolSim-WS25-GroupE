

#include "Container/ContainerFactory.h"
#include "Container/ContainerType.h"
#include "Container/ParticleContainer.h"
#include "Generator/DiscGenerator.h"
#include "Simulation/SimulationFactory.h"
#include "inputReader/Arguments.h"
#include "inputReader/InputReader.h"
#include "utils/logging.hpp"
/**
 * @brief Main entry point of the molecular dynamics simulation
 * @param argc Number of command line arguments
 * @param argsv Array of command line argument strings
 * @return EXIT_SUCCESS (0) on successful completion, -1 on error
 */
int main(int argc, char *argsv[]) {
  // Initialize logging (console + simulation.log)
  logging::init_logging();

  SPDLOG_INFO("MolSim starting...");

  Arguments args;
  inputReader::parseArguments(argc, argsv, args);

  auto container = ContainerFactory::createContainer(args);
  auto &particles = *container;

  SPDLOG_INFO("Creating simulation (input='{}')", args.inputFile);

  auto simulation = SimulationFactory::createSimulation(args, particles);

  SPDLOG_INFO("Starting simulation run...");
  simulation->runSimulation();
  SPDLOG_INFO("Simulation finished. Output written. Terminating.");

  return 0;
}
