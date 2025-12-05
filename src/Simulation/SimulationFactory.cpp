/**
 * @file SimulationFactory.cpp
 * @brief Implementation of the SimulationFactory.
 */
#include "SimulationFactory.h"

#include <memory>

#include "MoleculeSimulation.h"
#include "PlanetSimulation.h"


namespace SimulationFactory {
auto createSimulation(const SimulationConfig &cfg, Container &particles) -> std::unique_ptr<Simulation> {
  switch (cfg.sim_type) {
    case SimulationType::Planet:
      return std::make_unique<PlanetSimulation>(cfg, particles);
    case SimulationType::Molecule:
      return std::make_unique<MoleculeSimulation>(cfg, particles);
    default:
      // already checked in parseType, shouldn't be reached
      return std::make_unique<MoleculeSimulation>(cfg, particles);
  }
}
}  // namespace SimulationFactory