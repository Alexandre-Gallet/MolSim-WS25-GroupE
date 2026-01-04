//
// Created by darig on 12/29/2025.
//
#pragma once

#include "Container/Container.h"
class Thermostat {
 public:
  Thermostat(double t_init, size_t dimensions, size_t n_thermostat, double t_target, double delta_t,
             bool brownian_motion);
  void apply(Container &particles, size_t step);
  void initializeBrownianMotion(Container &particles) const;
  void calculateKineticEnergy(Container &particles);
  void calculateTemperature(const Container &particles);
  void calculateScalingFactor();

 private:
  double t_init;
  double delta_t;
  double t_target = -1.0;
  size_t dimensions;
  size_t n_thermostat;
  bool brownian_motion;
  double current_temperature = 0.0;
  double kinetic_energy = 0.0;
  double scaling_factor = 1.0;
};
