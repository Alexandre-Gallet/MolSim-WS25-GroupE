#include "LennardJones.h"

#include <cmath>

#include "../Container/LinkedCellContainer.h"
#include "../Container/ParticleContainer.h"
#include "../utils/ArrayUtils.h"
#include "utils/OpenMPCompat.h"

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
    // Skip interactions involving wall particles
    if (p1.getType() == 1 || p2.getType() == 1) return;

    const auto mixed = lookupPair(p1.getType(), p2.getType());
    calc(p1, p2, mixed.epsilon24, mixed.sigma6);
  };

  // -----------------------------
  // Choose traversal strategy
  // -----------------------------
  if (parallel_method_ == ParallelMethod::PairStatic && OpenMPCompat::enabled()) {
    // Strategy A: thread-local accumulation + static distribution of base cells.
    if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
      const std::size_t n = particles.size();
      const int T = std::max(1, OpenMPCompat::maxThreads());

      // Thread-local force buffers: [tid][particle_index] -> (fx,fy,fz)
      std::vector<double> fx(static_cast<std::size_t>(T) * n, 0.0);
      std::vector<double> fy(static_cast<std::size_t>(T) * n, 0.0);
      std::vector<double> fz(static_cast<std::size_t>(T) * n, 0.0);

      auto visitor = [this, &lookupPair, n, T, &fx, &fy, &fz](Particle &p1, Particle &p2, int tid) {
        // Skip interactions involving wall particles
        if (p1.getType() == 1 || p2.getType() == 1) return;

        if (tid < 0 || tid >= T) tid = 0;

        const std::size_t i = static_cast<std::size_t>(p1.getOwnedIndex());
        const std::size_t j = static_cast<std::size_t>(p2.getOwnedIndex());
        if (i >= n || j >= n) return;

        const auto mixed = lookupPair(p1.getType(), p2.getType());

        double dfx, dfy, dfz;
        computeForceComponents(p1, p2, mixed.epsilon24, mixed.sigma6, dfx, dfy, dfz);

        const std::size_t base = static_cast<std::size_t>(tid) * n;

        const bool p1_owned = !p1.isGhost();
        const bool p2_owned = !p2.isGhost();

        if (p1_owned) {
          fx[base + i] += dfx;
          fy[base + i] += dfy;
          fz[base + i] += dfz;
        }
        if (p2_owned) {
          fx[base + j] -= dfx;
          fy[base + j] -= dfy;
          fz[base + j] -= dfz;
        }
      };

      lc->forEachPairParallelStatic(visitor);

      // Reduce thread-local buffers into particle forces
      for (auto &p : particles) {
        // Do not add any accumulated interaction forces to wall particles
        if (p.getType() == 1) continue;

        const std::size_t idx = static_cast<std::size_t>(p.getOwnedIndex());
        if (idx >= n) continue;

        double sx = 0.0, sy = 0.0, sz = 0.0;
        for (int t = 0; t < T; ++t) {
          const std::size_t base = static_cast<std::size_t>(t) * n;
          sx += fx[base + idx];
          sy += fy[base + idx];
          sz += fz[base + idx];
        }
        p.addF(sx, sy, sz);
      }

      return;  // done
    }

    // Fallback (non-linked-cell container): keep correctness
    // (No efficient neighbor traversal available here.)
    particles.forEachPair([this, &lookupPair](Particle &p1, Particle &p2) {
      if (p1.getType() == 1 || p2.getType() == 1) return;
      const auto mixed = lookupPair(p1.getType(), p2.getType());
      calc(p1, p2, mixed.epsilon24, mixed.sigma6);
    });
    return;
  }

  if (parallel_method_ == ParallelMethod::CellDynamic && OpenMPCompat::enabled()) {
    if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
      const std::size_t n = particles.size();
      const int T = std::max(1, OpenMPCompat::maxThreads());

      std::vector<double> fx(static_cast<std::size_t>(T) * n, 0.0);
      std::vector<double> fy(static_cast<std::size_t>(T) * n, 0.0);
      std::vector<double> fz(static_cast<std::size_t>(T) * n, 0.0);

      auto visitorDyn = [this, &lookupPair, n, T, &fx, &fy, &fz](Particle &p1, Particle &p2, int tid) {
        if (tid < 0 || tid >= T) tid = 0;

        // Skip wall particles in pair interactions (keep your rule consistent).
        if (p1.getType() == 1 || p2.getType() == 1) return;

        const std::size_t i = static_cast<std::size_t>(p1.getOwnedIndex());
        const std::size_t j = static_cast<std::size_t>(p2.getOwnedIndex());
        if (i >= n || j >= n) return;

        const auto mixed = lookupPair(p1.getType(), p2.getType());

        double dfx, dfy, dfz;
        computeForceComponents(p1, p2, mixed.epsilon24, mixed.sigma6, dfx, dfy, dfz);

        const std::size_t base = static_cast<std::size_t>(tid) * n;
        fx[base + i] += dfx;
        fy[base + i] += dfy;
        fz[base + i] += dfz;
        fx[base + j] -= dfx;
        fy[base + j] -= dfy;
        fz[base + j] -= dfz;
      };

      lc->forEachPairCellDynamic(visitorDyn);

      // Reduction: only add to non-wall particles (your fix)
      for (auto &p : particles) {
        if (p.getType() == 1) continue;
        const std::size_t idx = static_cast<std::size_t>(p.getOwnedIndex());
        if (idx >= n) continue;

        double sx = 0.0, sy = 0.0, sz = 0.0;
        for (int t = 0; t < T; ++t) {
          const std::size_t base = static_cast<std::size_t>(t) * n;
          sx += fx[base + idx];
          sy += fy[base + idx];
          sz += fz[base + idx];
        }
        p.addF(sx, sy, sz);
      }

      return;
    }

    // fallback if not linked-cell
    particles.forEachPair(visitor);
    return;
  }

  // Default: existing serial traversal (your curre<nt code)
  if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
    lc->forEachPair(visitor);
  } else if (auto *pc = dynamic_cast<ParticleContainer *>(&particles)) {
    pc->forEachPair(visitor);
  } else {
    particles.forEachPair(visitor);
  }
}

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
