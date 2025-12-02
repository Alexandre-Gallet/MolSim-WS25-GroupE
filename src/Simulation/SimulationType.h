#pragma once
#include <iostream>
#include <string>

/**
 * Class to differentiate between the different simulation types
 */
enum class SimulationType { Planet, Molecule };

inline SimulationType parse_type(const std::string &sim_type) {
  if (sim_type == "planet" || sim_type == "Planet") {
    return SimulationType::Planet;
  }
  if (sim_type == "molecule" || sim_type == "Molecule") {
    return SimulationType::Molecule;
  }
  std::cerr << "Invalid simulation type";
  return SimulationType::Molecule;
}
