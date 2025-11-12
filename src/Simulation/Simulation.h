/**
 *@file Simulation.h
 */

#pragma once
/**
 * @brief Abstract class defining a common interface for different simulations
 */
class Simulation {
 public:
  explicit Simulation() = default;
  virtual ~Simulation() = default;
  virtual void runSimulation() = 0;
};