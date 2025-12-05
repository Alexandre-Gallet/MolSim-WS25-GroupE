/**
 * @file ContainerFactory.h
 * @brief Factory class for creating containers
 */
#pragma once

#include "../inputReader/SimulationConfig.h"
#include "Container.h"

namespace ContainerFactory {
/**
 *
 * @param cfg Argument struct containing the container type parsed as input
 * @return a new container of the specified type
 */
std::unique_ptr<Container> createContainer(SimulationConfig &cfg);
}  // namespace ContainerFactory
