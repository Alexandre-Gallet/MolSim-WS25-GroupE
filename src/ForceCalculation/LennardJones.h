/**
 *@file LennardJones.h
 */
#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#include "../Container/Particle.h"
#include "ForceCalculation.h"

#include "../inputReader/SimulationConfig.h"
/**
 * Class used to compute forces using the Lennard-Jones formulas
 */
class LennardJones : public ForceCalculation {
  double epsilon{};
  double sigma{};
  std::array<double, 3> gravity_{0.0, 0.0, 0.0};
  std::unordered_map<int, std::pair<double, double>> type_params_;

 public:
  LennardJones();
  ~LennardJones() override;
  // getters for parameters and potential
  [[nodiscard]] double getEpsilon() const { return epsilon; }
  [[nodiscard]] double getSigma() const { return sigma; }
  void setEpsilon(double eps) { this->epsilon = eps; }
  void setSigma(double sig) { this->sigma = sig; }
  void setGravity(const std::array<double, 3> &g) { gravity_ = g; }
  [[nodiscard]] const std::array<double, 3> &getGravity() const { return gravity_; }
  void setTypeParameters(const std::vector<LJTypeParams> &params) {
    type_params_.clear();
    for (const auto &p : params) {
      type_params_[p.type] = {p.epsilon, p.sigma};
    }
  }
  [[nodiscard]] double calculateU(const Particle &p1, const Particle &p2) const;
  /**
   * @brief Calculates the forces using the Lennard-Jones formulas
   * @param particles Particle container on which the calculations are performed
   */
  void calculateF(Container &particles) override;
  /**
   * @brief Calculate the force between two particles using Lennard-Jones formula
   * @param p1 First particle
   * @param p2 Second particle
   * @param epsilon
   * @param sigma
   */
  static void calc(Particle &p1, Particle &p2, double epsilon, double sigma);
};
