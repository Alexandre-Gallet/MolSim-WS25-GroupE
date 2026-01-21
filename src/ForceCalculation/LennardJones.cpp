#include "LennardJones.h"

#include <cmath>

#include "../Container/LinkedCellContainer.h"
#include "../Container/ParticleContainer.h"
#include "../utils/ArrayUtils.h"

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
  const auto &x1 = p1.getX();
  const auto &x2 = p2.getX();

  const double dx = x1[0] - x2[0];
  const double dy = x1[1] - x2[1];
  const double dz = x1[2] - x2[2];

  const double r2 = dx * dx + dy * dy + dz * dz;
  if (r2 == 0.0) {
    return 0.0;
  }
  const double invR2 = 1.0 / r2;
  const double sigma2 = sigma * sigma;
  const double sr2 = sigma2 * invR2;
  const double sr6 = sr2 * sr2 * sr2;
  return 4.0 * epsilon * (sr6 * sr6 - sr6);
}
void LennardJones::calculateF(Container &particles) {
  for (auto &p : particles) {
    // initialize to 0 so the simulation runs as expected
    p.setOldF(p.getF());
    if (p.getType() != 1) {
      auto gravityForce = ArrayUtils::elementWiseScalarOp(p.getM(), gravity_, std::multiplies<>());
      p.setF(gravityForce);
    } else {
      p.setF({0., 0., 0.});
    }
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
  auto lookup = [this](int type) {
    if (type >= 0 && type < static_cast<int>(type_params_dense_set_.size()) && type_params_dense_set_[type]) {
      return type_params_dense_[type];
    }
    auto it = type_params_.find(type);
    if (it != type_params_.end()) return it->second;
    return std::make_pair(epsilon, sigma);
  };

  auto lookupPair = [this, &lookup](int type1, int type2) {
    if (pair_params_dense_stride_ > 0 && type1 >= 0 && type2 >= 0 && type1 < pair_params_dense_stride_ &&
        type2 < pair_params_dense_stride_) {
      const int idx = type1 * pair_params_dense_stride_ + type2;
      if (pair_params_dense_set_[idx]) {
        return pair_params_dense_[idx];
      }
    }
    const auto params1 = lookup(type1);
    const auto params2 = lookup(type2);
    const auto mixed = mixParameters(params1, params2);
    const double sigma2 = mixed.second * mixed.second;
    const double sigma6 = sigma2 * sigma2 * sigma2;
    const double epsilon24 = 24.0 * mixed.first;
    return LennardJones::LJPairParams{epsilon24, sigma6};
  };

  // Define the visitor once (so we don't rebuild lambdas inside the hot path)
  auto visitor = [this, &lookupPair](Particle &p1, Particle &p2) {
    const auto mixed = lookupPair(p1.getType(), p2.getType());
    calc(p1, p2, mixed.epsilon24, mixed.sigma6);
  };

  // Call templated forEachPair on the *concrete* container to avoid std::function overhead
  if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
    lc->forEachPair(visitor);  // should bind to the templated overload (no std::function)
  } else if (auto *pc = dynamic_cast<ParticleContainer *>(&particles)) {
    pc->forEachPair(visitor);  // same idea
  } else {
    // Fallback: keep correctness if a different container type is used
    particles.forEachPair(visitor);  // will wrap into std::function (old behavior)
  }
}
/* Old version of LennarJones::Calc changed for serial optimization
void LennardJones::calc(Particle &p1, Particle &p2, double epsilon24, double sigma6) {
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
void LennardJones::calc(Particle &p1, Particle &p2, double epsilon24, double sigma6) {
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

void LennardJones::calc(Particle &p1, Particle &p2, double epsilon24, double sigma6) {
  // Cache particle state
  const auto &x1 = p1.getX();
  const auto &x2 = p2.getX();

  // Compute displacement components explicitly
  const double dx = x1[0] - x2[0];
  const double dy = x1[1] - x2[1];
  const double dz = x1[2] - x2[2];

  // r^2 with cutoff to avoid division by zero
  const double r2 = std::max(dx * dx + dy * dy + dz * dz, 1e-12);

  const double invR2 = 1.0 / r2;

  const double invR6 = invR2 * invR2 * invR2;
  const double sr6 = sigma6 * invR6;

  const double scalar = epsilon24 * invR2 * sr6 * (2.0 * sr6 - 1.0);

  // Apply forces (Newton's 3rd law)
  const double fx = scalar * dx;
  const double fy = scalar * dy;
  const double fz = scalar * dz;
  p1.addF(fx, fy, fz);
  p2.addF(-fx, -fy, -fz);
}
