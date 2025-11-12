
#include "LennardJones.h"

#include "../utils/ArrayUtils.h"

LennardJones::LennardJones() = default;
LennardJones::~LennardJones() = default;

double LennardJones::calculateU(const Particle &p1, const Particle &p2) const {
  double norm = std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 6);
  double potential = 4 * epsilon * norm * (norm - 1);
  return potential;
}
void LennardJones::calculateF(ParticleContainer &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  //Use pair iterator to calculates forces between each pair of particles
  particles.forEachPair([this](Particle &p1, Particle &p2) { calc(p1, p2, epsilon, sigma); });
}
void LennardJones::calc(Particle &p1, Particle &p2, double epsilon, double sigma) {
  double norm = ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>()));
  double scalar = 24 * epsilon / std::pow(norm, 2) * (std::pow(sigma / norm, 6)) * (1 - std::pow(sigma / norm, 6));
  std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(
      scalar, ArrayUtils::elementWisePairOp(p2.getX(), p1.getX(), std::minus<>()), std::multiplies<>());
  //Set the new values making use of Newton's third law
  p1.setF(ArrayUtils::elementWisePairOp(p1.getF(), newF, std::plus<>()));
  p2.setF(ArrayUtils::elementWisePairOp(p2.getF(), newF, std::minus<>()));
}
