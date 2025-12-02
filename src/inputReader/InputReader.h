/**
 * @file InputReader.h
 * @brief Namespace for command line argument parsing
 */
#pragma once

#include "Arguments.h"

namespace inputReader {
/**
 * @brief function to extract the values of the parameters used in the simulation
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @param args parsed values for each argument
 */
void parseArguments(int argc, char *argv[], Arguments &args);
}  // namespace inputReader
