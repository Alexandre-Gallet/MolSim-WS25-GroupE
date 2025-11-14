/**
* @file MolSim.cpp
 * @brief Main entry point for the Molecular Dynamics simulation
 */

#include "ParticleContainer.h"
#include "Simulation/SimulationFactory.h"
#include "inputReader/Arguments.h"
#include "inputReader/InputReader.h"
#include "logging.hpp"
#include <spdlog/spdlog.h>

/// Container holding all particles in the simulation
ParticleContainer particles;

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

    SPDLOG_INFO("Creating simulation (input='{}')", args.inputFile);

    auto simulation = SimulationFactory::createSimulation(args.sim_type, args, particles);

    SPDLOG_INFO("Starting simulation run...");
    simulation->runSimulation();
    SPDLOG_INFO("Simulation finished. Output written. Terminating.");

    return 0;
}
