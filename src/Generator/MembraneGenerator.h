/**
 * @file MembraneGenerator.h
 * @brief Grid-based generator that also wires neighbor relations for the membrane simulation.
 */
#pragma once

#include <vector>

#include "Container/Container.h"
#include "inputReader/SimulationConfig.h"

class MembraneGenerator {
 public:
  /**
   * @brief Create a rectangular membrane grid and set neighbor references.
   * @return Flat list of particle pointers in (x-major, then y, then z) order.
   */
  static std::vector<Particle *> generate(Container &container, const MembraneConfig &cfg);

  /**
   * @brief Access particle pointer by logical grid indices.
   */
  static Particle *particleAt(const std::vector<Particle *> &grid, const std::array<int, 3> &dims, int i, int j, int k);

 private:
  static std::size_t linearIndex(int i, int j, int k, const std::array<int, 3> &dims);
};
