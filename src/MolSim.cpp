/**
 * @file MolSim.cpp
 * @brief Main entry point for the Molecular Dynamics simulation
 */

// #include <bits/valarray_after.h>
#include <iostream>

#include "ParticleContainer.h"
#include "Simulation/SimulationFactory.h"
#include "inputReader/Arguments.h"
#include "inputReader/InputReader.h"

/// Container holding all particles in the simulation
ParticleContainer particles;

/**
 * @brief Main entry point of the molecular dynamics simulation
 * @param argc Number of command line arguments
 * @param argsv Array of command line argument strings
 * @return EXIT_SUCCESS (0) on successful completion, -1 on error
 */
int main(int argc, char *argsv[]) {
  Arguments args;
  inputReader::parseArguments(argc, argsv, args);
  auto simulation = SimulationFactory::createSimulation(SimulationType::Planet, args, particles);
  simulation->runSimulation();

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}