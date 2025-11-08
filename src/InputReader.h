/**
 * @file InputReader.h
 * @brief Namespace for command line argument parsing
 */
#pragma once
#include <string>

struct Arguments {
  char *inputFile;
  double t_start = 0.0;
  double t_end = 1000.0;
  double delta_t = 0.014;
  std::string outputFormat = "VTK";
};
namespace inputReader {
/**
 * @brief function for parsing the arguments
 * @param argc number of arguments
 * @param argv arguments read from command line
 * @param args argument values to be used in the simulation
 */
void parseArguments(int argc, char *argv[], Arguments &args);
}  // namespace inputReader
