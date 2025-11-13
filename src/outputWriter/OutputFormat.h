#pragma once
#include <string>
/**
 * Class to differentiate between the different output types
 */
enum class OutputFormat { XYZ, VTK };

inline OutputFormat parse_output(const std::string &output_format) {
  if (output_format == "XYZ") {
    return OutputFormat::XYZ;
  } else if (output_format == "VTK") {
    return OutputFormat::VTK;
  }
  return OutputFormat::XYZ;
}
