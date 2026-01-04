//
// Created by darig on 12/29/2025.
//

#include "Thermostat.h"

#include "ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

Thermostat::Thermostat(double t_init, size_t dimensions, size_t n_thermostat, double t_target, double delta_t,
                       bool brownian_motion)
    : t_init(t_init),
      delta_t(delta_t),
      t_target(t_target),
      dimensions(dimensions),
      n_thermostat(n_thermostat),
      brownian_motion(brownian_motion) {
  if (t_target < 0) {
    this->t_target = t_init;
  } else {
    this->t_target = t_target;
  }
}
void Thermostat::apply(Container &particles, size_t step) {
  if (step % n_thermostat != 0) {
    return;
  }
  if (step == 0 && brownian_motion) {
    initializeBrownianMotion(particles);
  }
  calculateKineticEnergy(particles);
  calculateTemperature(particles);
  if (current_temperature == t_target) {
    return;
  }
  calculateScalingFactor();
  for (auto p : particles) {
    std::array<double, 3> vel = ArrayUtils::elementWiseScalarOp(scaling_factor, p.getV(), std::multiplies<>());
    p.setV(vel);
  }
}
void Thermostat::initializeBrownianMotion(Container &particles) const {
  for (auto p : particles) {
    double factor = std::sqrt(t_init / p.getM());
    p.setV(maxwellBoltzmannDistributedVelocity(factor, dimensions));
  }
}
void Thermostat::calculateKineticEnergy(Container &particles) {
  double kin_en = 0.;
  for (const auto &p : particles) {
    kin_en += ArrayUtils::L2Norm(p.getV()) * ArrayUtils::L2Norm(p.getV()) * p.getM() / 2;
  }
  kinetic_energy = kin_en;
}
void Thermostat::calculateTemperature(const Container &particles) {
  current_temperature =
      2 * kinetic_energy / (static_cast<double>(dimensions) * static_cast<double>(particles.size()) * 1);
}
void Thermostat::calculateScalingFactor() {
  if (current_temperature == 0.) {
    return;
  }
  double diff = t_target - current_temperature;
  if (std::abs(diff) > delta_t) {
    if (diff > 0.) {
      diff = delta_t;
    } else {
      diff = -delta_t;
    }
  }
  double t_new = current_temperature + diff;
  scaling_factor = std::sqrt(t_new / current_temperature);
}
