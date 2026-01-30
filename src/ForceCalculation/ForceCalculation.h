/**
 *@file ForceCalculation.h
 */
#pragma once

#include "../Container/Container.h"
#include "../Container/LinkedCellContainer.h"
#include "../Container/ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

#ifdef _OPENMP
#include <omp.h>
#endif
/**
 * This abstract class defines a common interface for different ways of calculating the forces
 */
class ForceCalculation {
 public:
  // default constructor
  ForceCalculation() = default;
  // default destructor
  virtual ~ForceCalculation() = default;
  /**
   * @brief Function for calculating the force for each particle, implemented by each sub-class
   * @param particles Particle container on which the calculations are performed
   */
  virtual void calculateF(Container &particles) = 0;
  /**
   * @brief Function for calculating the position updates of the particles
   * @param particles Particle container on which the calculations are performed
   * @param delta_t Time step
   */
  static void calculateX(Container &particles, double delta_t) {
    // calculate the position updates using the methods in the ArrayUtils class
    // since the formulas are the same regardless of simulation, the method is included in the base class
    for (auto &p : particles) {
      if (p.getType() == 1) continue;
      p.setX(ArrayUtils::elementWisePairOp(
          p.getX(),
          ArrayUtils::elementWisePairOp(
              ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
              ArrayUtils::elementWiseScalarOp(pow(delta_t, 2) / (2 * p.getM()), p.getF(), std::multiplies<>()),
              std::plus<>()),
          std::plus<>()));
    }
  }

  /**
   * @brief Parallel position update for owned particles (OpenMP if available).
   */
  static void calculateXParallel(Container &particles, double delta_t) {
#ifdef _OPENMP
    if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
      const std::size_t count = lc->size();
      #pragma omp parallel for schedule(static)
      for (std::size_t i = 0; i < count; ++i) {
        auto &p = lc->ownedParticle(i);
        if (p.getType() == 1) continue;
        p.setX(ArrayUtils::elementWisePairOp(
            p.getX(),
            ArrayUtils::elementWisePairOp(
                ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
                ArrayUtils::elementWiseScalarOp((delta_t * delta_t) / (2 * p.getM()), p.getF(), std::multiplies<>()),
                std::plus<>()),
            std::plus<>()));
      }
      return;
    }

    if (auto *pc = dynamic_cast<ParticleContainer *>(&particles)) {
      const std::size_t count = pc->size();
      #pragma omp parallel for schedule(static)
      for (std::size_t i = 0; i < count; ++i) {
        auto &p = pc->particleAt(i);
        if (p.getType() == 1) continue;
        p.setX(ArrayUtils::elementWisePairOp(
            p.getX(),
            ArrayUtils::elementWisePairOp(
                ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
                ArrayUtils::elementWiseScalarOp((delta_t * delta_t) / (2 * p.getM()), p.getF(), std::multiplies<>()),
                std::plus<>()),
            std::plus<>()));
      }
      return;
    }
#endif
    calculateX(particles, delta_t);
  }
  /**
   * @brief Function for calculating the velocities of the particles
   * @param particles Particle container on which the calculations are performed
   * @param delta_t Time step
   */
  static void calculateV(Container &particles, double delta_t) {
    // calculate the forces using the methods in the ArrayUtils class
    // since the formulas are the same regardless of simulation, the method is included in the base class
    for (auto &p : particles) {
      if (p.getType() == 1) continue;
      p.setV(ArrayUtils::elementWisePairOp(
          p.getV(),
          ArrayUtils::elementWiseScalarOp(delta_t / (2 * p.getM()),
                                          ArrayUtils::elementWisePairOp(p.getOldF(), p.getF(), std::plus<>()),
                                          std::multiplies<>()),
          std::plus<>()));
    }
  }

  /**
   * @brief Parallel velocity update for owned particles (OpenMP if available).
   */
  static void calculateVParallel(Container &particles, double delta_t) {
#ifdef _OPENMP
    if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
      const std::size_t count = lc->size();
      #pragma omp parallel for schedule(static)
      for (std::size_t i = 0; i < count; ++i) {
        auto &p = lc->ownedParticle(i);
        if (p.getType() == 1) continue;
        p.setV(ArrayUtils::elementWisePairOp(
            p.getV(),
            ArrayUtils::elementWiseScalarOp(delta_t / (2 * p.getM()),
                                            ArrayUtils::elementWisePairOp(p.getOldF(), p.getF(), std::plus<>()),
                                            std::multiplies<>()),
            std::plus<>()));
      }
      return;
    }

    if (auto *pc = dynamic_cast<ParticleContainer *>(&particles)) {
      const std::size_t count = pc->size();
      #pragma omp parallel for schedule(static)
      for (std::size_t i = 0; i < count; ++i) {
        auto &p = pc->particleAt(i);
        if (p.getType() == 1) continue;
        p.setV(ArrayUtils::elementWisePairOp(
            p.getV(),
            ArrayUtils::elementWiseScalarOp(delta_t / (2 * p.getM()),
                                            ArrayUtils::elementWisePairOp(p.getOldF(), p.getF(), std::plus<>()),
                                            std::multiplies<>()),
            std::plus<>()));
      }
      return;
    }
#endif
    calculateV(particles, delta_t);
  }
};
