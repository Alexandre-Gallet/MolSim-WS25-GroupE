/**
 * @file InputReader.h
 * @brief Namespace for command line argument parsing
 */
#pragma once

#include "Arguments.h"

namespace inputReader {

/**
 * @brief Print help message for running the simulation.
 *
 * This function prints usage description, describing expected command line arguments
 *
 */
void printUsage();

/**
 * @brief Extract the values of the parameters used in the simulation.
 *
 * As of Assignment3 the command line arguments are:
 *   - <config_file>: Path to the YAML configuration file.
 *   - -h / --help  : Print help and exit.
 *
 * On invalid input print usage
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @param args Parsed values for each argument (currently only inputFile is set)
 */
void parseArguments(int argc, char *argv[], Arguments &args);

}  // namespace inputReader