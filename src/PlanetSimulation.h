
#pragma once

#include "ParticleContainer.h"
#include "Simulation.h"
#include "inputReader/Arguments.h"

class PlanetSimulation : public Simulation {
  char *inputFile;
  double t_start;
  double t_end;
  double delta_t;
  OutputFormat outputFormat;
  ParticleContainer &particles;

 public:
  [[nodiscard]] char *getInputFile() const { return inputFile; }
  [[nodiscard]] double getTStart() const { return t_start; }
  [[nodiscard]] double getTEnd() const { return t_end; }
  [[nodiscard]] double getDeltaT() const { return delta_t; }
  [[nodiscard]] OutputFormat getOutputFormat() const { return outputFormat; }
  [[nodiscard]] ParticleContainer &getParticles() const { return particles; }
  explicit PlanetSimulation(const Arguments &args, ParticleContainer &particles);
  ~PlanetSimulation() override = default;
  void runSimulation() override;
  static void plotParticles(ParticleContainer &particles, int iteration, OutputFormat format);
};