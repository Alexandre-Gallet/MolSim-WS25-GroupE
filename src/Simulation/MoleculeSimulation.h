/**
 *@file MoleculeSimulation.h
 */
#pragma once

#include "Container/Container.h"
#include "Simulation.h"
#include "inputReader/Arguments.h"
/**
 * Implementation of the molecule simulation using Lennard-Jones formulas
 */
class MoleculeSimulation : public Simulation {
  Arguments &args;
  Container &particles;

 public:
  explicit MoleculeSimulation(Arguments &args, Container &particles);
  ~MoleculeSimulation() override = default;
  /**
   *@brief main function for running the simulation
   */
  void runSimulation() override;
  static void plotParticles(Container &particles, int iteration, OutputFormat format);
};