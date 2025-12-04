/**
* @file SimulationFactory.h
 * @brief Factory for creating Simulation instances based on configuration.
 */
#pragma once
#include <memory>
#include "Simulation.h"
#include "inputReader/SimulationConfig.h"
#include "ParticleContainer.h"

/**
 * @brief Factory class responsible for creating concrete Simulation instances.
 */
class SimulationFactory {
public:
    /**
     * @brief Create a new Simulation instance based on the given configuration.
     *
     * @param cfg Simulation configuration (type, parameters, cuboids, etc.)
     * @param particles Reference to the particle container used by the simulation
     * @return std::unique_ptr<Simulation> Newly created simulation instance
     */
    static std::unique_ptr<Simulation> createSimulation(const SimulationConfig &cfg,
                                                        ParticleContainer &particles);
};
