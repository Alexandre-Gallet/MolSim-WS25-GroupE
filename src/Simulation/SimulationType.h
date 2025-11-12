#pragma once
#include <string>

/**
 * Class to differentiate between the different simulation types
 */
enum class SimulationType { Planet, LennardJones };

inline SimulationType parse_type(const std::string &sim_type) {
  if (sim_type == "Planet") {
    return SimulationType::Planet;
  } else if (sim_type == "LennardJones") {
    return SimulationType::LennardJones;
  }
  return SimulationType::LennardJones;
}
