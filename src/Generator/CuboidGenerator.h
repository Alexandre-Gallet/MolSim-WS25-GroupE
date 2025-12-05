  // CuboidGenerator.h
#pragma once
#include <array>

#include "Container/ParticleContainer.h"
#include "ParticleGenerator.h"
#include "utils/MaxwellBoltzmannDistribution.h"

/**
 * @brief Cuboid generator for particles.
 *
 * Can be used both via the static helper function generateCuboid(...)
 * and as a concrete implementation of ParticleGenerator.
 */
class CuboidGenerator : public ParticleGenerator {
public:
  /**
   * @brief Constructor for object-oriented usage (virtual interface).
   *
   * @param origin         Lower-left-front corner of the cuboid.
   * @param numPerDim      Number of particles per dimension (N1, N2, N3).
   * @param dom_size       Size of the domain
   * @param h              Distance between adjacent particles (h).
   * @param mass           Mass of each particle.
   * @param baseVelocity   Initial velocity of each particle.
   * @param brownianMean   Mean value of the Brownian Motion.
   * @param type           Type/id of the particle.
   */
  CuboidGenerator(const std::array<double, 3> &origin, const std::array<std::size_t, 3> &numPerDim,
                  const std::array<double, 3> &dom_size, double h, double mass,
                  const std::array<double, 3> &baseVelocity, double brownianMean = 0.1, int type = 0)
      : origin_(origin),
        numPerDim_(numPerDim),
        dom_size_(dom_size),
        h_(h),
        mass_(mass),
        baseVelocity_(baseVelocity),
        brownianMean_(brownianMean),
        type_(type) {}

  /**
   * @brief Implementation of the ParticleGenerator interface.
   *
   * Generates the cuboid and inserts particles into the container.
   */
  void generate(Container &container) const override;

  /**
   * @brief Static helper method that mirrors your old API.
   *
   * This keeps existing call sites working:
   *   CuboidGenerator::generateCuboid(container, ...);
   */
  static void generateCuboid(Container &container, const std::array<double, 3> &origin,
                             const std::array<std::size_t, 3> &numPerDim, const std::array<double, 3> &dom_size,
                             double h, double mass, const std::array<double, 3> &baseVelocity,
                             double brownianMean = 0.1, int type = 0);

 private:
  std::array<double, 3> origin_;
  std::array<std::size_t, 3> numPerDim_;
  const std::array<double, 3> &dom_size_;
  double h_;
  double mass_;
  std::array<double, 3> baseVelocity_;
  double brownianMean_;
  int type_;
};
