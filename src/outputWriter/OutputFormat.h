#pragma once
#include <stdexcept>
#include <string>
/**
 * Class to differentiate between the different output types
 */
// TODO: ENUMs should always be in all cap letters. Including Checkpoint.
// TODO: If you change this it has to be changed everywhere including for example input files
enum class OutputFormat { XYZ, VTK, Checkpoint, NONE };
/**
 * @brief convert the given command line parameter into the respective OutputFormat type
 * @param output_format type of output read from the command line
 * @return ObjectFormat object of the given type
 */
inline OutputFormat parse_output(const std::string &output_format) {
  if (output_format == "XYZ" || output_format == "xyz") {
    return OutputFormat::XYZ;
  } else if (output_format == "VTK" || output_format == "vtk") {
    return OutputFormat::VTK;
  } else if (output_format == "CHECKPOINT" || output_format == "checkpoint" || output_format == "STATE" ||
             output_format == "state") {
    return OutputFormat::Checkpoint;
  } else if (output_format == "NONE" || output_format == "none") {
    return OutputFormat::NONE;
  }
  // throw error if there is no match
  throw std::runtime_error("Invalid output_format '" + output_format +
                           "'. Valid values are: XYZ, VTK, CHECKPOINT, NONE.");
}
