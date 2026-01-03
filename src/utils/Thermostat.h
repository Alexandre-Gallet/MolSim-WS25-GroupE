//
// Created by darig on 12/29/2025.
//
# pragma once

#include "Container/Container.h"
class Thermostat {
  public:
  Thermostat(Container &particles, double T_init, size_t dimensions, size_t n_thermostat, double T_target, double delta_T, bool brownian_motion);
  ~Thermostat();
  void apply(Container &particles, size_t step, size_t dimensions);
  void initializeBrownianMotion() const;
  double calculateKineticEnergy();
  double calculateTemperature();
  void calculateScalingFactor();

private:
  Container &particles;
  double T_init;
  double delta_T;
  double T_target{-1};
  size_t dimensions;
  size_t n_thermostat;
  bool brownian_motion;
};
