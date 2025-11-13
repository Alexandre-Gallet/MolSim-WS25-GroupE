

#include "StormerVerlet.h"

#include "../utils/ArrayUtils.h"

StormerVerlet::StormerVerlet() = default;
StormerVerlet::~StormerVerlet() = default;

void StormerVerlet::calc(Particle &p1, Particle &p2) {
  double norm = std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 3);
  double scalar = p1.getM() * p2.getM() / norm;
  std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(
      scalar, ArrayUtils::elementWisePairOp(p2.getX(), p1.getX(), std::minus<>()), std::multiplies<>());
  // Set the new values making use of Newton's third law
  p1.setF(ArrayUtils::elementWisePairOp(p1.getF(), newF, std::plus<>()));
  p2.setF(ArrayUtils::elementWisePairOp(p2.getF(), newF, std::minus<>()));
}
void StormerVerlet::calculateF(ParticleContainer &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  // Use pair iterator to calculates forces between each pair of particles
  particles.forEachPair([](Particle &p1, Particle &p2) { calc(p1, p2); });
}