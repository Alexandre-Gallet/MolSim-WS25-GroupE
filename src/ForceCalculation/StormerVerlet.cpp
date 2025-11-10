

#include "StormerVerlet.h"

#include "../utils/ArrayUtils.h"

StormerVerlet::StormerVerlet() = default;
StormerVerlet::~StormerVerlet() = default;

void StormerVerlet::calc(Particle &p1, Particle &p2) {
  double norm = std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 3);
  double scalar = p1.getM() * p2.getM() / norm;
  std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(
      scalar, ArrayUtils::elementWisePairOp(p2.getX(), p1.getX(), std::minus<>()), std::multiplies<>());
  p1.setF(ArrayUtils::elementWisePairOp(p1.getF(), newF, std::plus<>()));
  p2.setF(ArrayUtils::elementWisePairOp(p2.getF(), newF, std::minus<>()));
}
void StormerVerlet::calculateF(ParticleContainer &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }
  particles.forEachPair([](Particle &p1, Particle &p2) { calc(p1, p2); });
}
void StormerVerlet::calculateX(ParticleContainer &particles, double delta_t) {
    // calculate the position updates using the methods in the ArrayUtils class

    for (auto &p : particles) {
      p.setX(ArrayUtils::elementWisePairOp(
          p.getX(),
          ArrayUtils::elementWisePairOp(
              ArrayUtils::elementWiseScalarOp(delta_t, p.getV(), std::multiplies<>()),
              ArrayUtils::elementWiseScalarOp(pow(delta_t, 2) / (2 * p.getM()), p.getF(), std::multiplies<>()),
              std::plus<>()),
          std::plus<>()));
    }
}
void StormerVerlet::calculateV(ParticleContainer &particles, double delta_t) {
  // calculate the forces using the methods in the ArrayUtils class

  for (auto &p : particles) {
    p.setV(ArrayUtils::elementWisePairOp(
        p.getV(),
        ArrayUtils::elementWiseScalarOp(delta_t / (2 * p.getM()),
                                        ArrayUtils::elementWisePairOp(p.getOldF(), p.getF(), std::plus<>()),
                                        std::multiplies<>()),
        std::plus<>()));
  }
}