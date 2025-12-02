
#pragma once
#include "../inputReader/Arguments.h"
#include "Container.h"
#include "ContainerType.h"
namespace ContainerFactory {
// TODO edit comments
/**
 * @brief creates a new simulation of the specified type
 * @param type Type of simulation, default: Molecule
 * @param args Arguments parsed from input
 * @param particles Container where all particles are stored
 * @return Simulation object of the given type
 */
std::unique_ptr<Container> createContainer(Arguments &args);
}  // namespace ContainerFactory
