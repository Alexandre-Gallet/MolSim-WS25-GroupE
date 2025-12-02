#pragma once
#include <string>
/**
 * Class to differentiate between the different output types
 */
enum class OutputFormat { XYZ, VTK };
/**
 * @brief convert the given command line parameter into the respective OutputFormat type
 * @param output_format type of output read from the command line
 * @return ObjectFormat object of the given type
 */
inline OutputFormat parse_output(const std::string &output_format) {
  if (output_format == "XYZ") {
    return OutputFormat::XYZ;
  } else if (output_format == "VTK") {
    return OutputFormat::VTK;
  }
  return OutputFormat::XYZ;
}
