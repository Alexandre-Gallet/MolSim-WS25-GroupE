/**
 * @file SimulationConfig.h
 * @brief Data Transfer Object storing simulation configuration parameters.
 */
#pragma once

#include <array>
#include <string>
#include <vector>

#include "Container/ContainerType.h"
#include "Container/LinkedCellContainer.h"
#include "Cuboid.h"
#include "Simulation/SimulationType.h"
#include "outputWriter/OutputFormat.h"
#include "utils/Thermostat.h"

/// Disc definition
struct Disc {
  std::array<double, 3> center{};        // [x,y,z]
  int radiusCells = 0;                   // integer count
  double hDisc = 0.0;                    // mesh width
  double mass = 1.0;                     // particle mass
  std::array<double, 3> baseVelocity{};  // [vx,vy,vz]
  int typeDisc = 0;                      // particle type
};

/// Thermostat definition
struct ThermostatConfig {
  double t_init;
  double t_target;
  double delta_t;
  size_t n_thermostat;
  size_t dimensions;
  bool brownian_motion;
  bool enable_thermostat = false; // Check if a thermostat should be used, so that old simulations still function
};

/**
 * @brief Bundles all simulation configuration options.
 */
struct SimulationConfig {
  // --- General simulation parameters ---
  SimulationType sim_type = SimulationType::Molecule;
  double t_start = 0.0;
  double t_end = 1000.0;
  double delta_t = 0.014;

#ifdef ENABLE_VTK_OUTPUT
  OutputFormat output_format = OutputFormat::VTK;
#else
  OutputFormat output_format = OutputFormat::XYZ;
#endif

  int write_frequency = 10;

  // --- Cuboids ---
  std::vector<Cuboid> cuboids;

  // --- Discs
  std::vector<Disc> discs;

  ContainerType containerType = ContainerType::Cell;  // containerType where all

  double rCutoff = 0.0;  // cutoff radius

  std::array<double, 3> domainSize{};  // simulation domain size

  //
  std::array<BoundaryCondition, 6> boundaryConditions{};  // 6 boundaries

  // --- Thermostat
  ThermostatConfig thermostat;
};