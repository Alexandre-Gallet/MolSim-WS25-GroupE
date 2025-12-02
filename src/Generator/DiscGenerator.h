#pragma once

#include <array>
#include "ParticleContainer.h"

/**
 * @brief Generates a 2D disc of particles in the XY-plane.
 *
 * Can be used in two ways:
 *   1. Object-oriented usage: create DiscGenerator object and call generate().
 *   2. Static helper: call DiscGenerator::generateDisc(...) directly.
 */
class DiscGenerator {
public:
  /**
   * @brief Object-oriented constructor.
   */
  DiscGenerator(const std::array<double, 3>& center,
                int radiusCells,
                double h,
                double mass,
                const std::array<double, 3>& baseVelocity,
                int type)
      : center_(center),
        radiusCells_(radiusCells),
        h_(h),
        mass_(mass),
        baseVelocity_(baseVelocity),
        type_(type) {}

  /**
   * @brief Object-oriented API: generates disc using stored parameters.
   */
  void generate(ParticleContainer& container) const;

  /**
   * @brief Static helper that generates a disc without constructing an object.
   *
   * @param container     ParticleContainer to fill
   * @param center        Center of the disc
   * @param radiusCells   Radius in lattice cells (integer)
   * @param h             Spacing between particles
   * @param mass          Particle mass
   * @param baseVelocity  Initial velocity
   * @param type          Particle type
   */
  static void generateDisc(ParticleContainer& container,
                           const std::array<double, 3>& center,
                           int radiusCells,
                           double h,
                           double mass,
                           const std::array<double, 3>& baseVelocity,
                           int type);

private:
  std::array<double, 3> center_;
  int radiusCells_;
  double h_;
  double mass_;
  std::array<double, 3> baseVelocity_;
  int type_;
};
