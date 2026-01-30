/**
 * @file SimulationConfig.h
 * @brief Data Transfer Object storing simulation configuration parameters.
 */
#pragma once

#include <array>
#include <cstdint>
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
  bool enable_thermostat = false;  // Check if a thermostat should be used, so that old simulations still function
};

/// Nano scale thermostat definition
struct NSThermostatConfig {
  double t_init;
  double t_target;
  double delta_t;
  size_t n_thermostat;
  size_t dimensions;
  bool brownian_motion;
  bool enable_thermostat = false;  // Check if a thermostat should be used, so that old simulations still function
};

/// Lennard-Jones parameters for a specific particle type.
struct LJTypeParams {
  int type = 0;
  double epsilon = 5.0;
  double sigma = 1.0;
};

/// Membrane-specific parameters
struct MembraneConfig {
  std::array<double, 3> origin{15.0, 15.0, 1.5};      // starting position of lower-left-front corner
  std::array<int, 3> n{50, 50, 1};                    // particles per dimension
  double h = 2.2;                                     // mesh width
  double mass = 1.0;                                  // mass per particle
  std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};  // initial velocity
  double epsilon = 1.0;                               // WCA epsilon
  double sigma = 1.0;                                 // WCA sigma
  double k = 300.0;                                   // spring stiffness
  double r0 = 2.2;                                    // rest length for direct neighbors
  double pull_force = 0.8;                            // constant upward force
  double pull_until = 150.0;                          // active time for upward pull
  std::vector<std::array<int, 2>> pull_indices{{{17, 24}}, {{17, 25}}, {{18, 24}}, {{18, 25}}};
};

/// Parallelization strategy selection
enum class ParallelStrategy : uint8_t { None, Force, Integrate };

/**
 * @brief Bundles all simulation configuration options.
 */
struct SimulationConfig {
  // --- General simulation parameters ---
  SimulationType sim_type = SimulationType::Molecule;
  double t_start = 0.0;
  double t_end = 1000.0;
  double delta_t = 0.014;

  // remove all compile flag hacking
  OutputFormat output_format = OutputFormat::XYZ;

  int write_frequency = 10;

  // Optional restart from checkpoint file
  bool restart_from_checkpoint = false;
  std::string checkpoint_file;

  // External acceleration (e.g., gravity)
  std::array<double, 3> gravity{0.0, 0.0, 0.0};

  // Default Lennard-Jones parameters (used if no per-type override)
  double lj_epsilon = 5.0;
  double lj_sigma = 1.0;
  // Optional per-type Lennard-Jones parameters
  std::vector<LJTypeParams> lj_types;

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

  // --- NS Thermostat
  NSThermostatConfig ns_thermostat;

  // --- Membrane (Assignment 4)
  MembraneConfig membrane;

  // --- Parallelization strategy
  /// Selects the parallelization strategy ("none", "force", "integrate").
  ParallelStrategy parallel_strategy = ParallelStrategy::Force;
};
