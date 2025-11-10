/**
 *@file ForceCalculation.h
 */
#pragma once

#include "../ParticleContainer.h"
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
};