/**
 * @file ContainerFactory.h
 * @brief Factory class for creating containers
 */
#pragma once

#include "../inputReader/Arguments.h"
#include "Container.h"

namespace ContainerFactory {
/**
 *
 * @param args Argument struct containing the container type parsed as input
 * @return a new container of the specified type
 */
std::unique_ptr<Container> createContainer(Arguments &args);
}  // namespace ContainerFactory
