

#include "SimulationFactory.h"

#include <memory>

#include "MoleculeSimulation.h"
#include "PlanetSimulation.h"

std::unique_ptr<Simulation> SimulationFactory::createSimulation(SimulationType type, Arguments &args,
                                                                ParticleContainer &particles) {
  switch (type) {
    case SimulationType::Molecule: {
      return std::make_unique<MoleculeSimulation>(args, particles);
    }
    default: {
      return std::make_unique<PlanetSimulation>(args, particles);
    }
  }
}
