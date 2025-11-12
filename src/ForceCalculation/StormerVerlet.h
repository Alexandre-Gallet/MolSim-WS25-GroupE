/**
 *@file StormerVerlet.h
 */
#pragma once

#include "../ParticleContainer.h"
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
  void calculateF(ParticleContainer &particles) override;
  static void calculateX(ParticleContainer &particles, double delta_t);
  static void calculateV(ParticleContainer &particles, double delta_t);
};