/**
 *@file MoleculeSimulation.h
 */
#pragma once

#include "../inputReader/Cuboid.h"
#include "Simulation.h"
#include "inputReader/Arguments.h"
/**
 * Implementation of the molecule simulation using Lennard-Jones formulas
 */
class MoleculeSimulation : public Simulation {
  Arguments &args;
  ParticleContainer &particles;
 public:
  explicit MoleculeSimulation(Arguments &args, ParticleContainer &particles);
  ~MoleculeSimulation() override = default;
  /**
   *@brief main function for running the simulation
   */
  void runSimulation() override;
  static void plotParticles(ParticleContainer &particles, int iteration, OutputFormat format);
};