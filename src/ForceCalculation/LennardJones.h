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
  [[nodiscard]] double getEpsilon() const { return epsilon; }
  [[nodiscard]] double getSigma() const { return sigma; }
  [[nodiscard]] double calculateU(const Particle &p1, const Particle &p2) const;
  void calculateF(ParticleContainer &particles) override;
  static void calc(Particle &p1, Particle &p2, double epsilon, double sigma);
};
