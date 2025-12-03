#pragma once

#include "Container/ContainerType.h"
#include "Cuboid.h"
#include "Simulation/SimulationType.h"
#include "outputWriter/OutputFormat.h"

// Struct with all possible command line parameters and their default values
struct Arguments {
  SimulationType sim_type;
  ContainerType cont_type = ContainerType::Cell;
  char *inputFile;
  double t_start = 0.0;
  double t_end = 1000.0;
  double delta_t = 0.014;
  double r_cutoff = 3.;
  std::array<double, 3> domain_size = {180., 90., 1};
#ifdef ENABLE_VTK_OUTPUT
  OutputFormat output_format = OutputFormat::VTK;
#else
  OutputFormat output_format = OutputFormat::XYZ;
#endif
};
