/**
 * @file SimulationFactory.h
 * @brief Factory for creating Simulation instances based on configuration.
 */
#pragma once
#include <memory>

#include "Container/ParticleContainer.h"
#include "Simulation.h"
#include "inputReader/SimulationConfig.h"

/**
 * @brief Factory class responsible for creating concrete Simulation instances.
 */
namespace SimulationFactory {
/**
 * @brief creates a new simulation of the specified type
 * @param type Type of simulation, default: Molecule
 * @param args Arguments parsed from input
 * @param particles Container where all particles are stored
 * @return Simulation object of the given type
 */
std::unique_ptr<Simulation> createSimulation(const SimulationConfig &cfg, Container &particles);
}  // namespace SimulationFactory
