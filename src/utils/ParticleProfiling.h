/**
 * @file ParticleProfiling.h
 * @brief Class for computing the density- and velocity- profile of the particles
 */
#pragma once

#include "Container/Container.h"

/**
 * @class ParticleProfiling
 * @brief Used to analyze the particle density and velocity changes for the nano scale simulation
 */
class ParticleProfiling {
 public:
  /**
   * @brief divides the x-axis in n_bins bins and computes the average velocity and particle density for each of them
   * @param particles particles to be analyzed
   * @param x_axis length of the x_axis
   * @param y_axis length of the y_axis
   * @param z_axis length of the z_axis
   * @param n_bins number of bins to divide the x_axis in
   * @param iteration current simulation step
   */
  static void computeProfiling(const Container &particles, double x_axis, double y_axis, double z_axis, int n_bins,
                               int iteration);
};
