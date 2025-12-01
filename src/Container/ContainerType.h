#pragma once
#include <iostream>
#include <string>

/**
 * Class to differentiate between the different container types
 */
enum ContainerType : uint8_t { Particle, Cell };

inline auto parseType(const std::string &cont_type) -> ContainerType {
  if (cont_type == "particle" || cont_type == "Particle") {
    return ContainerType::Particle;
  }
  if (cont_type == "cell" || cont_type == "Cell") {
    return ContainerType::Cell;
  }
  std::cerr << "Invalid container type";
  return ContainerType::Cell;
}
