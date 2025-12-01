
#include "LennardJones.h"

#include "../utils/ArrayUtils.h"

LennardJones::LennardJones() = default;
LennardJones::~LennardJones() = default;

double LennardJones::calculateU(const Particle &p1, const Particle &p2) const {
  const double distance = ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>()));
  if (distance == 0.0) {
    return 0.0;
  }
  const double sr = sigma / distance;
  const double sr6 = std::pow(sr, 6);
  return 4.0 * epsilon * (sr6 * sr6 - sr6);
}
void LennardJones::calculateF(ParticleContainer &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  // Use pair iterator to calculates forces between each pair of particles
  particles.forEachPair([this](Particle &p1, Particle &p2) { calc(p1, p2, epsilon, sigma); });
}
void LennardJones::calc(Particle &p1, Particle &p2, double epsilon, double sigma) {
  const auto diff = ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>());
  const double distance = std::max(ArrayUtils::L2Norm(diff), 1e-12);
  const double invR2 = 1.0 / (distance * distance);
  const double sr = sigma / distance;
  const double sr6 = std::pow(sr, 6);
  const double scalar = 24.0 * epsilon * invR2 * sr6 * (2.0 * sr6 - 1.0);
  std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(scalar, diff, std::multiplies<>());
  // Set the new values making use of Newton's third law
  p1.setF(ArrayUtils::elementWisePairOp(p1.getF(), newF, std::plus<>()));
  p2.setF(ArrayUtils::elementWisePairOp(p2.getF(), newF, std::minus<>()));
}
