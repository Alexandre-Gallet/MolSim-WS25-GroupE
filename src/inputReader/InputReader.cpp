/**
 * @file InputReader.cpp
 * @brief Implementation of the input parser
 */
#include "InputReader.h"

#include <iostream>
#include <ostream>
#include <string>

#include "Arguments.h"
#include "../Simulation/SimulationType.h"
#include "outputWriter/OutputFormat.h"

/**
 * @brief function to check if two strings are equal
 * @param a first string
 * @param b second string
 * @return true if they're the same, false if not
 */
bool isequal(const std::string &a, const std::string &b) {
  if (a.size() != b.size()) return false;
  return std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

namespace inputReader {
/**
 * @brief print help message for running the simulation
 */
static void printUsage() {
  std::cout << "Usage: ./MolSim <input_file> <sim_type> [t_start] [t_end] [delta_t] [output_format]\n"
            << "Example: ./MolSim eingabe-sonne.txt 0 1000 0.014 VTK\n"
            << "Arguments:\n"
            << "\t<input_file>\t:\tPath to particle data (text file)\n"
            << "\t<sim_type>\t:\tSimulation type: planet or molecule (default molecule)\n"
            << "\t[-s | --t_start <value>]\t:\tStart time of the simulation (default 0)\n"
            << "\t[-e | --t_end <value>]\t:\tEnd time of the simulation (default 1000)\n"
            << "\t[-d | --delta_t <value>]\t:\tTime step (default 0.014)\n"
            << "\t[-f | --format <value>]\t:\tOutput format (default if enabled: 'VTK').\n"
            << "\t[-h | --help]\t:\tDisplay this help message\n";
}
/**
 * @brief function to extract the values of the parameters used in the simulation
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @param args parsed values for each argument
 */
void parseArguments(const int argc, char *argv[], Arguments &args) {
  if (argc < 2) {
    std::cerr << "Invalid number of arguments\n"
              << "Use -h or --help to display usage.\n";
    std::exit(EXIT_FAILURE);
  }
  //print the help message if necessary
  if (std::string arg = argv[1]; arg == "-h" || arg == "--help") {
    printUsage();
    std::exit(EXIT_SUCCESS);
  }
  args.inputFile = argv[1];
  if (argc < 3) {
    std::cerr << "Invalid number of arguments\n"
              << "Use -h or --help to display usage.\n";
    std::exit(EXIT_FAILURE);
  }
  args.sim_type = parse_type(argv[2]);
  //parse the arguments, checking the flags and saving the values for the simulation parameters
  for (int i = 3; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      printUsage();
      std::exit(EXIT_SUCCESS);
    }
    if (arg == "-s" || arg == "--t_start") {
      if (i + 1 < argc) {
        args.t_start = std::stod(argv[++i]);
      } else {
        std::cerr << "Missing value for \"-s\" argument\n";
        std::exit(EXIT_FAILURE);
      }
    } else if (arg == "-e" || arg == "--t_end") {
      if (i + 1 < argc) {
        args.t_end = std::stod(argv[++i]);
      } else {
        std::cerr << "Missing value for \"-e\" argument\n";
        std::exit(EXIT_FAILURE);
      }
    } else if (arg == "-d" || arg == "--delta_t") {
      if (i + 1 < argc) {
        args.delta_t = std::stod(argv[++i]);
      } else {
        std::cerr << "Missing value for \"-d\" argument\n";
        std::exit(EXIT_FAILURE);
      }
    } else if (arg == "-f" || arg == "--format") {
      if (i + 1 < argc) {
        if (!isequal(argv[i + 1], "VTK") && !isequal(argv[i + 1], "XYZ")) {
          std::cerr << "Incorrect value for \"-f\" argument\n";
          std::exit(EXIT_FAILURE);
        }
        args.output_format = parse_output(argv[++i]);
      } else {
        std::cerr << "Missing value for \"-f\" argument\n";
        std::exit(EXIT_FAILURE);
      }
    } else {
      printUsage();
      std::exit(EXIT_FAILURE);
    }
  }
  //check the validity of the given parameters
  if (args.t_start > args.t_end) {
    std::cerr << "Start time must be greater than end time\n";
    std::exit(EXIT_FAILURE);
  }
  if (args.delta_t < 0 || args.t_start < 0 || args.t_end < 0) {
    std::cerr << "Time step, start and end time must be greater than 0\n";
    std::exit(EXIT_FAILURE);
  }
}
}  // namespace inputReader