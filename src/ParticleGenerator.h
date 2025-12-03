#pragma once
#include <array>

#include "Container/ParticleContainer.h"
#include "inputReader/Arguments.h"
#include "utils/MaxwellBoltzmannDistribution.h"

class ParticleGenerator {
 public:
  /**
   * @brief Creates a cuboid of particles and inserts them into the given container.
   *
   * @param container  Container to fill with generated particles.
   * @param origin     Lower-left-front corner of the cuboid.
   * @param numPerDim  Number of particles per dimension (N1, N2, N3).
   * @param h    Distance between adjacent particles (h).
   * @param mass       Mass of each particle.
   * @param baseVelocity   Initial velocity of each particle.
   * @param type       Type/id of the particle.
   */
  static void generateCuboid(Container &container, const std::array<double, 3> &origin,
                             const std::array<size_t, 3> &numPerDim, const std::array<double, 3> &dom_size, double h,
                             double mass, const std::array<double, 3> &baseVelocity, double brownianMean = 0.1,
                             int type = 0);
};