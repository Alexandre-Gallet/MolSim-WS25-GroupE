/**
*@file LennardJones.h
 */
#pragma once

#include "ForceCalculation.h"
#include "ParticleContainer.h"
#include "Particle.h"
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
