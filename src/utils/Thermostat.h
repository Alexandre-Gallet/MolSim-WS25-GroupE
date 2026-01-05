/**
 * @file Thermostat.h
 * @brief Class for controlling the temperature of the simulation
 */
#pragma once

#include "Container/Container.h"

/**
 * @class Thermostat
 * @brief Models a thermostat which manages the temperature of the particle system
 */
class Thermostat {
 public:
  /**
   * @brief Constructs a Thermostat object.
   * @param t_init Initial temperature of the system.
   * @param dimensions The number of dimensions in the simulation (2 or 3).
   * @param n_thermostat Interval (in time steps) at which the thermostat is applied.
   * @param t_target The desired temperature.
   * @param delta_t Maximum allowed temperature change per thermostat application.
   * @param brownian_motion If true, initializes velocities using Maxwell-Boltzmann distribution at step 0.
   */
  Thermostat(double t_init, size_t dimensions, size_t n_thermostat, double t_target, double delta_t,
             bool brownian_motion);
  /**
   * @brief Applies the thermostat logic to the particle container.
   * @param particles Reference to the container holding the particles.
   * @param step The current iteration step of the simulation.
   */
  void apply(Container &particles, size_t step);
  /**
   * @brief Initializes particle velocities using a Maxwell-Boltzmann distribution.
   * Used to provide initial kinetic energy to a system starting from rest.
   * * @param particles Reference to the container to initialize.
   */
  void initializeBrownianMotion(Container &particles) const;
  /**
   * @brief Calculates the total kinetic energy of the system.
   * @param particles Reference to the container.
   */
  void calculateKineticEnergy(Container &particles);
  /**
   * @brief Calculates the current temperature of the system.
   * @param particles Reference to the container.
   */
  void calculateTemperature(const Container &particles);
  /**
   * @brief Computes the velocity scaling factor \f$ \beta \f$.
   * Ensures the temperature change does not exceed @ref delta_t.
   */
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
