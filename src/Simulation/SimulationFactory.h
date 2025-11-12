/**
 *@file SimulationFactory.h
 */

#pragma once

#include "../ParticleContainer.h"
#include "SimulationType.h"
#include "../inputReader/Arguments.h"
#include "Simulation.h"

class SimulationFactory {
 public:
  static Simulation *createSimulation(SimulationType type, const Arguments &args, ParticleContainer &particles);
};
