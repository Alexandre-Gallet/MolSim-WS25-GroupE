/**
 * @file SimulationFactory.cpp
 * @brief Implementation of the SimulationFactory.
 */
#include "SimulationFactory.h"

#include <memory>

#include "MoleculeSimulation.h"
#include "PlanetSimulation.h"

std::unique_ptr<Simulation> SimulationFactory::createSimulation(const SimulationConfig &cfg,
                                                                ParticleContainer &particles) {
  switch (cfg.sim_type) {
    case SimulationType::Molecule: {
      return std::make_unique<MoleculeSimulation>(cfg, particles);
    }
    default: {
      return std::make_unique<PlanetSimulation>(cfg, particles);
    }
  }
}
