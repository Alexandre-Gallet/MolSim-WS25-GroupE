/**
 *@file SimulationFactory.h
 */

#pragma once

#include <memory>

#include "../Container/ParticleContainer.h"
#include "../inputReader/Arguments.h"
#include "Simulation.h"
#include "SimulationType.h"
/**
 * Create desired simulation of the given type
 */
class SimulationFactory {
 public:
  /**
   * @brief creates a new simulation of the specified type
   * @param type Type of simulation, default: Molecule
   * @param args Arguments parsed from input
   * @param particles Container where all particles are stored
   * @return Simulation object of the given type
   */
  static std::unique_ptr<Simulation> createSimulation(SimulationType type, Arguments &args,
                                                      ParticleContainer &particles);
};
