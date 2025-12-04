/**
* @file SimulationConfig.h
 * @brief Data Transfer Object storing simulation configuration parameters.
 */
#pragma once

#include <string>
#include <vector>

#include "Cuboid.h"
#include "Simulation/SimulationType.h"
#include "outputWriter/OutputFormat.h"

/**
 * @brief Struct for all configuration options for a simulation run.
 *
 * This struct is filled by the YAML input reader.
 */
struct SimulationConfig {
    /// Type of simulation to run (planet, molecule, etc.)
    SimulationType sim_type = SimulationType::Molecule;

    /// Start time of the simulation.
    double t_start = 0.0;
    /// End time of the simulation.
    double t_end = 1000.0;
    /// Time step of the simulation.
    double delta_t = 0.014;

#ifdef ENABLE_VTK_OUTPUT
    /// Output format of the simulation.
    OutputFormat output_format = OutputFormat::VTK;
#else
    /// Output format of the simulation.
    OutputFormat output_format = OutputFormat::XYZ;
#endif

   /// Frequency of writing output (every n-th timestep).
    int write_frequency = 10;

    /// List of cuboids to be generated at the start of the simulation.
    std::vector<Cuboid> cuboids;
};