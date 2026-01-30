/**
 * @file MoleculeSimulation.h
 * @brief Molecule simulation implementation.
 */
#pragma once

#include "Container/Container.h"
#include "Generator/DiscGenerator.h"
#include "Simulation.h"
#include "inputReader/SimulationConfig.h"
#include "utils/Thermostat.h"

/**
 * @brief Simulation class for molecular dynamics (Lennard-Jones).
 */
class MoleculeSimulation : public Simulation {
 public:
  /**
   * @brief Construct a new MoleculeSimulation.
   *
   * @param cfg Simulation configuration (read from YAML)
   * @param particles Reference to the particle container
   */
  MoleculeSimulation(const SimulationConfig &cfg, Container &particles);

  /**
   * @brief Run the molecular dynamics simulation.
   *
   * The behavior depends on SimulationConfig::parallel_strategy:
   * - Force: parallelize Lennard-Jones force calculation.
   * - Integrate: parallelize position/velocity integration steps.
   * - None: fully serial.
   */
  void runSimulation() override;

  /**
   * @brief Plot Particles using writer classes (VTK, XYZ) for later visualization
   */
  static void plotParticles(Container &particles, int iteration, OutputFormat format);

  /// Copy of simulation configuration. This is very cheap!
  SimulationConfig cfg_;

  /// Reference to the particle container shared by the simulation system. Copying this would be extremly expensive here
  /// a refrence is better
  Container &particles_;
};
