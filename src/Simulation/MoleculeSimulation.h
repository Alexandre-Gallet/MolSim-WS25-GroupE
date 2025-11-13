
#pragma once

#include "../inputReader/Cuboid.h"
#include "Simulation.h"
#include "inputReader/Arguments.h"

class MoleculeSimulation : public Simulation {
  Arguments &args;
  ParticleContainer &particles;
 public:
  explicit MoleculeSimulation(Arguments &args, ParticleContainer &particles);
  ~MoleculeSimulation() override;
  void runSimulation() override;
  static void plotParticles(ParticleContainer &particles, int iteration, OutputFormat format);
};