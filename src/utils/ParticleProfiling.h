/**
 * @file ParticleProfiling.h
 * @brief Class for computing the density- and velocity- profile of the particles
 */
#pragma once

#include "Container/Container.h"

class ParticleProfiling {
public:
  static void computeProfiling(const Container &particles, double x_axis, double y_axis, double z_axis, int n_bins);
};

