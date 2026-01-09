#include "LennardJones.h"

#include <cmath>

#include "../utils/ArrayUtils.h"

#include "../Container/LinkedCellContainer.h"

#include "../Container/ParticleContainer.h"

namespace {
std::pair<double, double> mixParameters(const std::pair<double, double> &a, const std::pair<double, double> &b) {
  const double sigma = 0.5 * (a.second + b.second);
  const double epsilon = std::sqrt(a.first * b.first);
  return {epsilon, sigma};
}
}  // namespace

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
void LennardJones::calculateF(Container &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    auto gravityForce = ArrayUtils::elementWiseScalarOp(p.getM(), gravity_, std::multiplies<>());
    p.setF(gravityForce);
  }

  /* Original code that got replaced for serial speedup
  // Use pair iterator to calculates forces between each pair of particles
  particles.forEachPair([this](Particle &p1, Particle &p2) {
    auto lookup = [this](int type) {
      auto it = type_params_.find(type);
      if (it != type_params_.end()) {
        return it->second;
      }
      return std::make_pair(epsilon, sigma);
    };
    auto params1 = lookup(p1.getType());
    auto params2 = lookup(p2.getType());
    const auto mixed = mixParameters(params1, params2);
    calc(p1, p2, mixed.first, mixed.second);
  });
  */
  // Define the visitor once (so we don't rebuild lambdas inside the hot path)
  auto visitor = [this](Particle &p1, Particle &p2) {
    // (optional micro-fix: move lookup outside visitor if you can, but not necessary for Opt 1)
    auto lookup = [this](int type) {
      auto it = type_params_.find(type);
      if (it != type_params_.end()) return it->second;
      return std::make_pair(epsilon, sigma);
    };

    auto params1 = lookup(p1.getType());
    auto params2 = lookup(p2.getType());
    const auto mixed = mixParameters(params1, params2);
    calc(p1, p2, mixed.first, mixed.second);
  };

  // Call templated forEachPair on the *concrete* container to avoid std::function overhead
  if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
    lc->forEachPair(visitor);          // should bind to the templated overload (no std::function)
  } else if (auto *pc = dynamic_cast<ParticleContainer *>(&particles)) {
    pc->forEachPair(visitor);          // same idea
  } else {
    // Fallback: keep correctness if a different container type is used
    particles.forEachPair(visitor);    // will wrap into std::function (old behavior)
  }


}
/* Old version of LennarJones::Calc changed for serial optimization
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
*/

/* Old version of LennardJones::Calc changed for serial optimization
void LennardJones::calc(Particle &p1, Particle &p2, double epsilon, double sigma) {
  // Cache particle state locally (reduce accessor calls)
  const auto &x1 = p1.getX();
  const auto &x2 = p2.getX();

  auto f1 = p1.getF();
  auto f2 = p2.getF();

  const auto diff =
      ArrayUtils::elementWisePairOp(x1, x2, std::minus<>());

  const double distance =
      std::max(ArrayUtils::L2Norm(diff), 1e-12);

  const double invR = 1.0 / distance;
  const double invR2 = invR * invR;

  const double sr = sigma * invR;
  const double sr2 = sr * sr;
  const double sr6 = sr2 * sr2 * sr2;

  const double scalar =
      24.0 * epsilon * invR2 * sr6 * (2.0 * sr6 - 1.0);

  const auto newF =
      ArrayUtils::elementWiseScalarOp(scalar, diff, std::multiplies<>());

  // Write forces back once
  p1.setF(ArrayUtils::elementWisePairOp(f1, newF, std::plus<>()));
  p2.setF(ArrayUtils::elementWisePairOp(f2, newF, std::minus<>()));
}
*/

void LennardJones::calc(Particle &p1, Particle &p2, double epsilon, double sigma) {
  // Cache particle state
  const auto &x1 = p1.getX();
  const auto &x2 = p2.getX();

  auto f1 = p1.getF();
  auto f2 = p2.getF();

  // Compute displacement components explicitly
  const double dx = x1[0] - x2[0];
  const double dy = x1[1] - x2[1];
  const double dz = x1[2] - x2[2];

  // r^2 with cutoff to avoid division by zero
  const double r2 = std::max(dx * dx + dy * dy + dz * dz, 1e-12);

  const double invR2 = 1.0 / r2;
  const double invR = std::sqrt(invR2);

  const double sr2 = (sigma * sigma) * invR2;
  const double sr6 = sr2 * sr2 * sr2;

  const double scalar =
      24.0 * epsilon * invR2 * sr6 * (2.0 * sr6 - 1.0);

  // Apply forces (Newton's 3rd law)
  f1[0] += scalar * dx;
  f1[1] += scalar * dy;
  f1[2] += scalar * dz;

  f2[0] -= scalar * dx;
  f2[1] -= scalar * dy;
  f2[2] -= scalar * dz;

  // Write back once
  p1.setF(f1);
  p2.setF(f2);
}


