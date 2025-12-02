

#include "SimulationFactory.h"

#include <memory>

#include "MoleculeSimulation.h"
#include "PlanetSimulation.h"

auto SimulationFactory::createSimulation(SimulationType type, Arguments &args, ParticleContainer &particles)
    -> std::unique_ptr<Simulation> {
  switch (type) {
    case SimulationType::Planet:
      return std::make_unique<PlanetSimulation>(args, particles);
    case SimulationType::Molecule:
      return std::make_unique<MoleculeSimulation>(args, particles);
    default:
      return std::make_unique<MoleculeSimulation>(args, particles);
  }
}