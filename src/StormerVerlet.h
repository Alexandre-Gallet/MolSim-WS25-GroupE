/**
 *@file StormerVerlet.h
 */
#pragma once

#include "ForceCalculation.h"
#include "ParticleContainer.h"
/**
 * Class used to compute forces using the Störmer-Verlet formulas
 */
class StormerVerlet : public ForceCalculation {
public:
  StormerVerlet();
  ~StormerVerlet() override;
  void calculateF(ParticleContainer &particles) override;
};