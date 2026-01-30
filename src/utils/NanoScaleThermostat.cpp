
#include "NanoScaleThermostat.h"

#include <spdlog/fmt/bundled/format.h>

#include "ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

NanoScaleThermostat::NanoScaleThermostat(double t_init, size_t dimensions, size_t n_thermostat, double t_target,
                                         double delta_t, bool brownian_motion)
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
void NanoScaleThermostat::apply(Container &particles, size_t step) {
  // only apply the thermostat every n_thermostat steps
  if (step % n_thermostat != 0) {
    return;
  }
  // if specified, initialize the brownian motion
  if (step == 0 && brownian_motion) {
    initializeBrownianMotion(particles);
  }
  // reset average_velocity in each application
  average_velocity = {0., 0., 0.};
  // calculate average velocity
  int particle_count = 0;
  for (const auto &p : particles) {
    if (p.getType() == 1) {
      continue;
    }
    average_velocity = ArrayUtils::elementWisePairOp(average_velocity, p.getV(), std::plus<>());
    particle_count++;
  }
  if (particle_count == 0) {
    return;
  }
  for (int i = 0; i < 3; i++) {
    average_velocity.at(i) /= particle_count;
  }

  calculateKineticEnergy(particles);
  calculateTemperature(particle_count);
  // if the current temperature has reached the desired one, don't update the velocities
  if (std::abs(current_temperature - t_target) < 1e-9) {
    return;
  }
  calculateScalingFactor();
  // update velocities with the scaling factor
  for (auto &p : particles) {
    if (p.getType() == 1) {
      continue;
    }
    std::array<double, 3> thermal_motion = ArrayUtils::elementWisePairOp(p.getV(), average_velocity, std::minus<>());
    std::array<double, 3> vel = ArrayUtils::elementWiseScalarOp(scaling_factor, thermal_motion, std::multiplies<>());
    p.setV(ArrayUtils::elementWisePairOp(vel, average_velocity, std::plus<>()));
  }
}
void NanoScaleThermostat::initializeBrownianMotion(Container &particles) const {
  for (auto &p : particles) {
    double factor = std::sqrt(t_init / p.getM());
    p.setV(maxwellBoltzmannDistributedVelocity(factor, dimensions));
  }
}
void NanoScaleThermostat::calculateKineticEnergy(Container &particles) {
  double kin_en = 0.;
  for (const auto &p : particles) {
    if (p.getType() == 1) {
      continue;
    }
    std::array<double, 3> thermal_motion = ArrayUtils::elementWisePairOp(p.getV(), average_velocity, std::minus<>());
    kin_en += ArrayUtils::L2Norm(thermal_motion) * ArrayUtils::L2Norm(thermal_motion) * p.getM() / 2;
  }
  kinetic_energy = kin_en;
}
void NanoScaleThermostat::calculateTemperature(int particle_count) {
  current_temperature = 2 * kinetic_energy / (static_cast<double>(dimensions) * particle_count * 1);
}
void NanoScaleThermostat::calculateScalingFactor() {
  // avoid division by null
  if (current_temperature == 0.) {
    return;
  }
  double diff = t_target - current_temperature;
  // if delta_t is specified, update the temperature gradually by at most delta_t, otherwise set the temperature
  // directly
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
