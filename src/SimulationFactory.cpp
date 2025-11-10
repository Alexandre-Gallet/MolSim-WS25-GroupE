

#include "SimulationFactory.h"

#include "PlanetSimulation.h"

Simulation *SimulationFactory::createSimulation(SimulationType type, const Arguments &args,
                                                ParticleContainer &particles) {
  switch (type) {
    case SimulationType::Planet:
      return new PlanetSimulation(args, particles);
      // TODO add lennard jones simulation
    case SimulationType::LennardJones:
      return new PlanetSimulation(args, particles);
    default:
      return new PlanetSimulation(args, particles);
  }
}