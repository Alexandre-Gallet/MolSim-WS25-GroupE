#include "StormerVerlet.h"

#include <spdlog/spdlog.h>

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

void StormerVerlet::calculateF(Container &particles) {
  // Reset forces
  for (auto &p : particles) {
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }

  SPDLOG_DEBUG("Recomputing gravitational forces for {} particles (Stormer-Verlet).", particles.size());

  // Use pair iterator to calculate forces between each pair of particles.
  // TRACE-level: extremely fine-grained debug, compiled out unless LOG_LEVEL=TRACE.
  particles.forEachPair([](Particle &p1, Particle &p2) {
    calc(p1, p2);
    SPDLOG_TRACE("Updated forces between a particle pair in Stormer-Verlet.");
  });
}