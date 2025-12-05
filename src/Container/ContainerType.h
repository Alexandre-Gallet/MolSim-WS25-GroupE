/**
 * @file ContainerType.h
 */
#pragma once

#include <spdlog/spdlog.h>

#include <iostream>
#include <string>

/**
 * Class to differentiate between the different container types
 */
enum class ContainerType { Particle, Cell };

inline auto parseContainerType(const std::string &cont_type) -> ContainerType {
  if (cont_type == "particle" || cont_type == "Particle") {
    return ContainerType::Particle;
  }
  if (cont_type == "cell" || cont_type == "Cell") {
    return ContainerType::Cell;
  }
  SPDLOG_ERROR("Invalid container type: {}", cont_type);
  return ContainerType::Cell;
}
