/**
 *@file ForceCalculation.h
 */
#pragma once

#include "../ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"
/**
 * This abstract class defines a common interface for different ways of calculating the forces
 */
class ForceCalculation {
 public:
  ForceCalculation() = default;
  virtual ~ForceCalculation() = default;
  /**
   * @brief Function for calculating the force for each particle
   * @param particles Particle container on which the calculations are performed
   */
  virtual void calculateF(ParticleContainer &particles) = 0;
  static void calculateX(ParticleContainer &particles, double delta_t) {
    // calculate the position updates using the methods in the ArrayUtils class

    for (auto &p : particles) {
      p.setX(ArrayUtils::elementWisePairOp(
          p.getX(),
          ArrayUtils::elementWisePairOp(
              ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
              ArrayUtils::elementWiseScalarOp(pow(delta_t, 2) / (2 * p.getM()), p.getF(), std::multiplies<>()),
              std::plus<>()),
          std::plus<>()));
    }

  }
};