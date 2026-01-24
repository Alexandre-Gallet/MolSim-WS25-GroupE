/**
 * @file MembraneSimulation.h
 * @brief Simulation driver for the membrane assignment.
 */
#pragma once

#include <vector>

#include "Container/LinkedCellContainer.h"
#include "ForceCalculation/MembraneForce.h"
#include "Simulation.h"
#include "inputReader/SimulationConfig.h"
#include "outputWriter/OutputFormat.h"

class MembraneSimulation : public Simulation {
 public:
  MembraneSimulation(const SimulationConfig &cfg, Container &particles);
  ~MembraneSimulation() override = default;

  void runSimulation() override;

 private:
  SimulationConfig cfg_;
  Container &particles_;
  MembraneForce membrane_force_;
  std::vector<Particle *> grid_;

  void plotParticles(Container &particles, int iteration, OutputFormat format);
  std::vector<Particle *> collectPullTargets() const;
};
