
#pragma once

#include "Simulation.h"
#include "../Cuboid.h"

class MoleculeSimulation : public Simulation {
  Cuboid cuboid;
  double t_start;
  double t_end;
  double delta_t;
  public:
  [[nodiscard]] double getTStart() const { return t_start; }
  [[nodiscard]] double getTEnd() const { return t_end; }
  [[nodiscard]] double getDeltaT() const { return delta_t; }
  explicit MoleculeSimulation() = default;
  ~MoleculeSimulation() override;
  void runSimulation() override;
};