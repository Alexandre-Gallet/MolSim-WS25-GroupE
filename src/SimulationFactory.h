/**
 *@file SimulationFactory.h
 */

#pragma once

#include "ParticleContainer.h"
#include "Simulation.h"
#include "SimulationType.h"
#include "inputReader/Arguments.h"

class SimulationFactory {
  public:
  static Simulation* createSimulation(SimulationType type, const Arguments &args, ParticleContainer &particles);
};
