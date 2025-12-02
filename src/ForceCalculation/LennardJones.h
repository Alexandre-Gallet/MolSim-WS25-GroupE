/**
 *@file LennardJones.h
 */
#pragma once

#include "../Particle.h"
#include "../ParticleContainer.h"
#include "ForceCalculation.h"
/**
 * Class used to compute forces using the Lennard-Jones formulas
 */
class LennardJones : public ForceCalculation {
  double epsilon{};
  double sigma{};

 public:
  LennardJones();
  ~LennardJones() override;
  // getters for parameters and potential
  [[nodiscard]] double getEpsilon() const { return epsilon; }
  [[nodiscard]] double getSigma() const { return sigma; }
  void setEpsilon(double eps) { this->epsilon = eps; }
  void setSigma(double sig) { this->sigma = sig; }
  [[nodiscard]] double calculateU(const Particle &p1, const Particle &p2) const;
  /**
   * @brief Calculates the forces using the Lennard-Jones formulas
   * @param particles Particle container on which the calculations are performed
   */
  void calculateF(ParticleContainer &particles) override;
  /**
   * @brief Calculate the force between two particles using Lennard-Jones formula
   * @param p1 First particle
   * @param p2 Second particle
   * @param epsilon
   * @param sigma
   */
  static void calc(Particle &p1, Particle &p2, double epsilon, double sigma);
};
