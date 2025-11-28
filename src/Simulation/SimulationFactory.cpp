

#include "SimulationFactory.h"

#include "MoleculeSimulation.h"
#include "PlanetSimulation.h"

Simulation *SimulationFactory::createSimulation(SimulationType type, Arguments &args, ParticleContainer &particles) {
  switch (type) {
    case SimulationType::Planet:
      return new PlanetSimulation(args, particles);
    case SimulationType::Molecule:
      return new MoleculeSimulation(args, particles);
    default:
      return new MoleculeSimulation(args, particles);
  }
}