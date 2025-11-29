#pragma once
#include <iostream>
#include <string>

/**
 * Class to differentiate between the different simulation types
 */
enum SimulationType : uint8_t { Planet, Molecule };

inline auto parseType(const std::string &sim_type) -> SimulationType {
  if (sim_type == "planet" || sim_type == "Planet") {
    return SimulationType::Planet;
  }
  if (sim_type == "molecule" || sim_type == "Molecule") {
    return SimulationType::Molecule;
  }
  std::cerr << "Invalid simulation type";
  return SimulationType::Molecule;
}
