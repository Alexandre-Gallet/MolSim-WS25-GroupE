
#pragma once

#include "../Container/ParticleContainer.h"
#include "../inputReader/Arguments.h"
#include "Simulation.h"

class PlanetSimulation : public Simulation {
  char *inputFile;
  double t_start;
  double t_end;
  double delta_t;
  OutputFormat outputFormat;
  Container &particles;

 public:
  [[nodiscard]] char *getInputFile() const { return inputFile; }
  [[nodiscard]] double getTStart() const { return t_start; }
  [[nodiscard]] double getTEnd() const { return t_end; }
  [[nodiscard]] double getDeltaT() const { return delta_t; }
  [[nodiscard]] OutputFormat getOutputFormat() const { return outputFormat; }
  [[nodiscard]] Container &getParticles() const { return particles; }
  explicit PlanetSimulation(const Arguments &args, Container &particles);
  ~PlanetSimulation() override = default;
  /**
   * @brief main function for running the simulation
   */
  void runSimulation() override;
  static void plotParticles(Container &particles, int iteration, OutputFormat format);
};