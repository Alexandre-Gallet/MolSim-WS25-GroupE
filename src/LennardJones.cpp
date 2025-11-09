
#include "LennardJones.h"
#include "utils/ArrayUtils.h"

 LennardJones::LennardJones() = default;
LennardJones::~LennardJones() = default;

double LennardJones::calculateU(Particle p1, Particle p2) {
  double norm =
    std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 6);
  double potential = 4 * epsilon * norm * (norm - 1);
  return potential;
}
void LennardJones::calculateF(ParticleContainer &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  for (auto &p1 : particles) {
    for (auto &p2 : particles) {
      if (!(p1 == p2)) {

      }
    }
  }
}

