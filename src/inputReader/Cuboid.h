#pragma once

#include "../Container/ParticleContainer.h"

// Struct with the given parameters of the cuboid and a container for storing the particles
struct Cuboid {
  std::array<double, 3> origin{};
  std::array<size_t, 3> numPerDim{};
  double h{0.0};
  double mass{1.0};
  std::array<double, 3> baseVelocity{};
  double brownianMean{0.1};
  int type{0};
};
