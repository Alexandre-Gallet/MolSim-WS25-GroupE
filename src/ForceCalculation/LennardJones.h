/**
 *@file LennardJones.h
 */
#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#include "../Container/Particle.h"
#include "../inputReader/SimulationConfig.h"
#include "ForceCalculation.h"
/**
 * Class used to compute forces using the Lennard-Jones formulas
 */
class LennardJones : public ForceCalculation {
  struct LJPairParams {
    double epsilon24{};
    double sigma6{};
  };

  double epsilon{};
  double sigma{};
  std::array<double, 3> gravity_{0.0, 0.0, 0.0};
  std::unordered_map<int, std::pair<double, double>> type_params_;
  std::vector<std::pair<double, double>> type_params_dense_;
  std::vector<bool> type_params_dense_set_;
  std::vector<LJPairParams> pair_params_dense_;
  std::vector<bool> pair_params_dense_set_;
  int pair_params_dense_stride_ = 0;
  bool enable_omp_forces_ = true;

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
  void setEnableOmpForces(bool enable) { enable_omp_forces_ = enable; }
  void setTypeParameters(const std::vector<LJTypeParams> &params) {
    type_params_.clear();
    int max_type = -1;
    bool all_non_negative = true;
    for (const auto &p : params) {
      type_params_[p.type] = {p.epsilon, p.sigma};
      if (p.type < 0) {
        all_non_negative = false;
      } else if (p.type > max_type) {
        max_type = p.type;
      }
    }

    constexpr int kDenseThreshold = 1024;
    if (all_non_negative && max_type >= 0 && max_type <= kDenseThreshold) {
      type_params_dense_.assign(max_type + 1, {});
      type_params_dense_set_.assign(max_type + 1, false);
      for (const auto &p : params) {
        type_params_dense_[p.type] = {p.epsilon, p.sigma};
        type_params_dense_set_[p.type] = true;
      }
      const int n = static_cast<int>(type_params_dense_.size());
      pair_params_dense_.assign(n * n, {});
      pair_params_dense_set_.assign(n * n, false);
      pair_params_dense_stride_ = n;
      for (int i = 0; i < n; ++i) {
        if (!type_params_dense_set_[i]) continue;
        for (int j = 0; j < n; ++j) {
          if (!type_params_dense_set_[j]) continue;
          const auto mixed = [&]() {
            const double sigma_m = 0.5 * (type_params_dense_[i].second + type_params_dense_[j].second);
            const double epsilon_m = std::sqrt(type_params_dense_[i].first * type_params_dense_[j].first);
            return std::make_pair(epsilon_m, sigma_m);
          }();
          const double sigma2 = mixed.second * mixed.second;
          const double sigma6 = sigma2 * sigma2 * sigma2;
          const double epsilon24 = 24.0 * mixed.first;
          const int idx = i * n + j;
          pair_params_dense_[idx] = {epsilon24, sigma6};
          pair_params_dense_set_[idx] = true;
        }
      }
    } else {
      type_params_dense_.clear();
      type_params_dense_set_.clear();
      pair_params_dense_.clear();
      pair_params_dense_set_.clear();
      pair_params_dense_stride_ = 0;
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
  static void calc(Particle &p1, Particle &p2, double epsilon24, double sigma6);
  static std::array<double, 3> calcPairForce(const Particle &p1, const Particle &p2, double epsilon24, double sigma6);
};
