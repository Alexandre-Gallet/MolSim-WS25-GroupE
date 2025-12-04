/**
* @file PlanetSimulation.h
 * @brief Simulation of gravitational planetary dynamics using Störmer-Verlet.
 */
#pragma once

#include "../ParticleContainer.h"
#include "../Simulation/Simulation.h"
#include "../inputReader/SimulationConfig.h"

/**
 * @brief Simulation class for gravitational planet motion.
 *
 */
class PlanetSimulation : public Simulation {
public:
    /**
     * @brief Construct a new PlanetSimulation.
     *
     * @param cfg YAML-based simulation configuration
     * @param particles Reference to particle container
     */
    PlanetSimulation(const SimulationConfig &cfg, ParticleContainer &particles);

    /**
     * @brief Run the simulation.
     */
    void runSimulation() override;

private:
    /**
     * @brief Write particle positions to an output file.
     *
     * @param particles Particle container
     * @param iteration Current iteration number
     * @param format Output format (XYZ or VTK)
     */
    void plotParticles(ParticleContainer &particles, int iteration, OutputFormat format);

    /// Copy of simulation configuration from YAML.
    SimulationConfig cfg_;

    /// Reference to the main particle container updated during simulation.
    ParticleContainer &particles_;
};