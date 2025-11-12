#pragma once

#include "Cuboid.h"
#include "outputWriter/OutputFormat.h"

//Struct with all possible command line parameters and their default values
struct Arguments {
  char *inputFile;
  double t_start = 0.0;
  double t_end = 1000.0;
  double delta_t = 0.014;
  std::vector<Cuboid> cuboids;
#ifdef ENABLE_VTK_OUTPUT
  OutputFormat output_format = OutputFormat::VTK;
#else
  OutputFormat outputFormat = OutputFormat::XYZ;
#endif
};
