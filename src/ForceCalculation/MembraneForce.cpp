#include "MembraneForce.h"

#include <cmath>
#include <functional>

#include "../Container/LinkedCellContainer.h"
#include "../Container/ParticleContainer.h"
#include "../utils/ArrayUtils.h"

// This force handler combines gravity, short-range Lennard-Jones repulsion, Hookean springs along the membrane mesh,
// and an optional constant pull on selected particles; the LJ cutoff is precomputed once for efficiency.
MembraneForce::MembraneForce(double epsilon, double sigma, double k, double r0, double pull_force, double pull_until,
                             const std::array<double, 3> &gravity)
    : epsilon_(epsilon),
      sigma_(sigma),
      k_(k),
      r0_(r0),
      pull_force_(pull_force),
      pull_until_(pull_until),
      gravity_(gravity) {
  // Precompute WCA cutoff^2 so pair checks can stay in squared-distance space.
  const double cutoff = std::pow(2.0, 1.0 / 6.0) * sigma_;
  cutoff2_ = cutoff * cutoff;
}

void MembraneForce::setPullTargets(const std::vector<Particle *> &targets) { pull_targets_ = targets; }

void MembraneForce::setCurrentTime(double time) { current_time_ = time; }

// Reinitialize each particle with gravity, then accumulate pairwise repulsion, spring forces along neighbor links,
// and the optional pulling force for marked particles.
void MembraneForce::calculateF(Container &particles) {
  for (auto &p : particles) {
    // Old force is preserved for the integrator; start new force at gravity only.
    p.setOldF(p.getF());
    auto gravityForce = ArrayUtils::elementWiseScalarOp(p.getM(), gravity_, std::multiplies<>());
    p.setF(gravityForce);
  }

  applyRepulsive(particles);
  applySprings(particles);
  applyPull();
}

void MembraneForce::applyRepulsive(Container &particles) {
  // Use container-specific pair iteration to avoid std::function overhead.
  auto visitor = [this](Particle &p1, Particle &p2) {
    const auto &x1 = p1.getX();
    const auto &x2 = p2.getX();

    const double dx = x1[0] - x2[0];
    const double dy = x1[1] - x2[1];
    const double dz = x1[2] - x2[2];

    const double r2 = dx * dx + dy * dy + dz * dz;
    // Skip distant pairs and guard against zero-distance blow-up.
    if (r2 >= cutoff2_ || r2 < kEps) {
      return;
    }

    const double invR2 = 1.0 / r2;
    const double sr2 = (sigma_ * sigma_) * invR2;
    const double sr6 = sr2 * sr2 * sr2;
    const double scalar = 24.0 * epsilon_ * invR2 * sr6 * (2.0 * sr6 - 1.0);

    // Apply equal and opposite forces to keep momentum conservation.
    auto f1 = p1.getF();
    auto f2 = p2.getF();

    f1[0] += scalar * dx;
    f1[1] += scalar * dy;
    f1[2] += scalar * dz;

    f2[0] -= scalar * dx;
    f2[1] -= scalar * dy;
    f2[2] -= scalar * dz;

    p1.setF(f1);
    p2.setF(f2);
  };

  if (auto *lc = dynamic_cast<LinkedCellContainer *>(&particles)) {
    lc->forEachPair(visitor);
  } else if (auto *pc = dynamic_cast<ParticleContainer *>(&particles)) {
    pc->forEachPair(visitor);
  } else {
    particles.forEachPair(visitor);
  }
}

void MembraneForce::applySprings(Container &particles) {
  for (auto &p : particles) {
    for (const auto &neighbor : p.getNeighbors()) {
      auto *q = neighbor.particle;
      if (q == nullptr) {
        continue;
      }
      if (&p > q) {
        continue;  // handle each edge once
      }

      const auto &x1 = p.getX();
      const auto &x2 = q->getX();

      const double dx = x2[0] - x1[0];
      const double dy = x2[1] - x1[1];
      const double dz = x2[2] - x1[2];

      const double dist2 = dx * dx + dy * dy + dz * dz;
      if (dist2 < kEps) {
        continue;
      }

      const double dist = std::sqrt(dist2);
      // Hooke force magnitude based on extension/compression along the edge.
      const double scalar = k_ * (dist - neighbor.rest_length) / dist;

      // Equal and opposite updates keep the symmetric neighbor graph stable.
      auto f1 = p.getF();
      auto f2 = q->getF();

      f1[0] += scalar * dx;
      f1[1] += scalar * dy;
      f1[2] += scalar * dz;

      f2[0] -= scalar * dx;
      f2[1] -= scalar * dy;
      f2[2] -= scalar * dz;

      p.setF(f1);
      q->setF(f2);
    }
  }
}

void MembraneForce::applyPull() {
  if (current_time_ > pull_until_) {
    return;
  }

  for (auto *p : pull_targets_) {
    if (p == nullptr) {
      continue;
    }
    // Constant upward nudge on the selected nodes.
    auto f = p->getF();
    f[2] += pull_force_;
    p->setF(f);
  }
}
