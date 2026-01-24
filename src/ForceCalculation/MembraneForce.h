/**
 * @file MembraneForce.h
 * @brief Harmonic spring + repulsive Lennard-Jones force for membrane simulation.
 */
#pragma once

#include <array>
#include <vector>

#include "../Container/Particle.h"
#include "ForceCalculation.h"

/**
 * @brief Force model for the membrane task:
 * - Harmonic springs between stored neighbors (different rest lengths for diagonal edges).
 * - Repulsive-only Lennard-Jones (WCA-style) to avoid self-penetration.
 * - Optional constant upward pull on selected particles for a limited time window.
 */
class MembraneForce : public ForceCalculation {
 public:
  MembraneForce(double epsilon, double sigma, double k, double r0, double pull_force, double pull_until,
                const std::array<double, 3> &gravity);

  /**
   * @brief Set particles that experience an upward pull.
   */
  void setPullTargets(const std::vector<Particle *> &targets);

  /**
   * @brief Set current simulation time (used to gate the upward pull).
   */
  void setCurrentTime(double time);

  /**
   * @brief Calculate forces on all particles.
   */
  void calculateF(Container &particles) override;

 private:
  double epsilon_;
  double sigma_;
  double k_;
  double r0_;
  double pull_force_;
  double pull_until_;
  double cutoff2_;
  double current_time_{0.0};
  std::array<double, 3> gravity_{};
  std::vector<Particle *> pull_targets_;

  void applyRepulsive(Container &particles);
  void applySprings(Container &particles);
  void applyPull();

  static constexpr double kEps = 1e-12;
};
