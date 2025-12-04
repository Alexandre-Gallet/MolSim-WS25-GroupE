/**
 * @file InputReader.cpp
 * @brief Implementation of the command line input parser
 */
#include "InputReader.h"

#include <iostream>
#include <ostream>
#include <string>
#include "Arguments.h"

namespace {

/**
 * @brief Helper function to check if two strings are equal, case-insensitive.
 *
 * @param a First string
 * @param b Second string
 * @return true if they're the same (ignoring case), false otherwise
 */
bool isequal(const std::string &a, const std::string &b) {
  if (a.size() != b.size()) return false;
  return std::equal(a.begin(), a.end(), b.begin(), [](char ca, char cb) {
    return std::tolower(ca) == std::tolower(cb);
  });
}

}  // namespace

namespace inputReader {

/**
 * @brief Print help message for running the simulation.
 */
void printUsage() {
  std::cout << "Usage: ./MolSim <config_file> [options]\n"
            << "Example: ./MolSim config.yml\n\n"
            << "Arguments:\n"
            << "\t<config_file>\t:\tPath to YAML configuration file.\n"
            << "\t-h | --help\t:\tDisplay this help message.\n\n"
            << "Note:\n"
            << "  All simulation parameters are specified in the YAML file.\n";
}

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
void parseArguments(const int argc, char *argv[], Arguments &args) {
  if (argc < 2) {
    std::cerr << "Invalid number of arguments.\n"
              << "Use -h or --help to display usage.\n";
    std::exit(EXIT_FAILURE);
  }

  // Print the help message if necessary
  if (std::string arg = argv[1]; isequal(arg, "-h") || isequal(arg, "--help")) {
    printUsage();
    std::exit(EXIT_SUCCESS);
  }

  // First non-option argument is treated as the YAML config file
  args.inputFile = argv[1];

  // If there are unexpected extra arguments, treat this as an error to keep the
  // interface clean for YAML-based configuration.
  if (argc > 2) {
    std::cerr << "Unexpected additional command line arguments.\n"
              << "For YAML-based input, only the configuration file is expected.\n\n";
    printUsage();
    std::exit(EXIT_FAILURE);
  }
}

}  // namespace inputReader