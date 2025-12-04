#include "DiscGenerator.h"

#include <cmath>

void DiscGenerator::generate(ParticleContainer &container) const {
  const double physicalRadius = static_cast<double>(radiusCells_) * h_;
  const double R2 = physicalRadius * physicalRadius;

  const std::size_t estimatedCount = static_cast<std::size_t>(3.14159 * radiusCells_ * radiusCells_);
  container.reserve(container.size() + estimatedCount);

  for (int i = -radiusCells_; i <= radiusCells_; ++i) {
    for (int j = -radiusCells_; j <= radiusCells_; ++j) {
      double dx = static_cast<double>(i) * h_;
      double dy = static_cast<double>(j) * h_;

      if (dx * dx + dy * dy <= R2) {
        std::array<double, 3> pos = {center_[0] + dx, center_[1] + dy, center_[2]};

        container.emplaceParticle(pos, baseVelocity_, mass_, type_);
      }
    }
  }
}

/**
 * Static wrapper — zero code duplication.
 */
void DiscGenerator::generateDisc(ParticleContainer &container, const std::array<double, 3> &center, int radiusCells,
                                 double h, double mass, const std::array<double, 3> &baseVelocity, int type) {
  DiscGenerator gen(center, radiusCells, h, mass, baseVelocity, type);
  gen.generate(container);
}
