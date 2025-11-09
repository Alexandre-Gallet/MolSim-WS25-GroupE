

#include "StormerVerlet.h"
#include "utils/ArrayUtils.h"

 StormerVerlet::StormerVerlet() = default;
 StormerVerlet::~StormerVerlet() = default;


void StormerVerlet::calculateF(ParticleContainer &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    p.setF({0., 0., 0.});
  }

  for (auto &p1 : particles) {
    for (auto &p2 : particles) {
      if (!(p1 == p2)) {  // calculate the forces according to the given formula in the lecture
        double norm =
            std::pow(ArrayUtils::L2Norm(ArrayUtils::elementWisePairOp(p1.getX(), p2.getX(), std::minus<>())), 3);
        double scalar = p1.getM() * p2.getM() / norm;
        std::array<double, 3> newF = ArrayUtils::elementWiseScalarOp(
            scalar, ArrayUtils::elementWisePairOp(p2.getX(), p1.getX(), std::minus<>()), std::multiplies<>());
        p1.setF(ArrayUtils::elementWisePairOp(p1.getF(), newF, std::plus<>()));
        // p2.setF(ArrayUtils::elementWisePairOp(p2.getF(),newF,  std::minus<>()));
      }
    }
  }
}