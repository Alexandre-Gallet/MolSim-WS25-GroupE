//
// Created by altay on 18.11.2025.
//

#ifndef PSEMOLDYN_DISC_H
#define PSEMOLDYN_DISC_H
#include "../ParticleContainer.h"

class DiscGenerator {

DiscGenerator(double h, ParticleContainer &container, double mass, int radiusCells, const std::array<double, 3> &center, const std::array<double, 3> &initialVel):
  center(center), initi



private:
  double h_;
  double mass_;
  int radiusCells_;
  std::array<double,3> center_;
  std::array<double,3> initialVel_;

};

#endif  // PSEMOLDYN_DISC_H
