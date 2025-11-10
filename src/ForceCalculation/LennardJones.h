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
  double epsilon;
  double sigma;

 public:
  LennardJones();
  ~LennardJones() override;
  double calculateU(Particle p1, Particle p2);
  void calculateF(ParticleContainer &particles) override;
};
