#pragma once
#include <string>
/**
 * Class to differentiate between the different output types
 */
enum class OutputFormat { XYZ, VTK, Checkpoint };
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
  }
  return OutputFormat::XYZ;
}
