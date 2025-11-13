#pragma once

#include "../ParticleContainer.h"

struct Cuboid {
  ParticleContainer &container;
  const std::array<double, 3> &origin;
  const std::array<size_t, 3> &numPerDim;
  double h;
  double mass;
  const std::array<double, 3> &baseVelocity;
  double brownianMean;
  int type;
};