/**
 *@file StormerVerlet.h
 */
#pragma once

#include "../Container/ParticleContainer.h"
#include "ForceCalculation.h"
/**
 * Class used to compute forces using the Störmer-Verlet formulas
 */
class StormerVerlet : public ForceCalculation {
 public:
  StormerVerlet();
  ~StormerVerlet() override;
  /**
   * @brief Computes the forces between two particles
   * @param p1 First particle
   * @param p2 Second particle
   */
  static void calc(Particle &p1, Particle &p2);
  /**
   * @brief Calculates the forces using the Störmer-Verlet formulas
   * @param particles Particle container on which the calculations are performed
   */
  void calculateF(Container &particles) override;
};