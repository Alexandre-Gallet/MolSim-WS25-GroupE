#pragma once

#include "Cuboid.h"
#include "Simulation/SimulationType.h"
#include "outputWriter/OutputFormat.h"

//Struct with all possible command line parameters and their default values
struct Arguments {
  SimulationType sim_type = SimulationType::Molecule;
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
