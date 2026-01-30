#pragma once
#include <spdlog/spdlog.h>

#include <iostream>
#include <string>

/**
 * Class to differentiate between the different simulation types
 */

enum class SimulationType { Planet, Molecule, Membrane };

inline auto parseSimType(const std::string &sim_type) -> SimulationType {
  if (sim_type == "planet" || sim_type == "Planet") {
    return SimulationType::Planet;
  }
  if (sim_type == "molecule" || sim_type == "Molecule") {
    return SimulationType::Molecule;
  }
  if (sim_type == "membrane" || sim_type == "Membrane") {
    return SimulationType::Membrane;
  }
  SPDLOG_ERROR("Invalid simulation type: {}", sim_type);
  return SimulationType::Molecule;
}
