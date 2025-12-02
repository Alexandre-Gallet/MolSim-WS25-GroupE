

#include "SimulationFactory.h"

#include <memory>

#include "MoleculeSimulation.h"
#include "PlanetSimulation.h"

namespace SimulationFactory {
auto createSimulation(Arguments &args, Container &particles) -> std::unique_ptr<Simulation> {
  switch (args.sim_type) {
    case SimulationType::Planet:
      return std::make_unique<PlanetSimulation>(args, particles);
    case SimulationType::Molecule:
      return std::make_unique<MoleculeSimulation>(args, particles);
    default:
      // already checked in parseType, shouldn't be reached
      return std::make_unique<MoleculeSimulation>(args, particles);
  }
}
}  // namespace SimulationFactory