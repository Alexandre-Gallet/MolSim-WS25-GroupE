#pragma once
#include <string>

#include "outputWriter/OutputFormat.h"

struct Arguments {
  char *inputFile;
  double t_start = 0.0;
  double t_end = 1000.0;
  double delta_t = 0.014;
#ifdef ENABLE_VTK_OUTPUT
  OutputFormat output_format = OutputFormat::VTK;
#else
  OutputFormat outputFormat = OutputFormat::XYZ;
#endif
};
