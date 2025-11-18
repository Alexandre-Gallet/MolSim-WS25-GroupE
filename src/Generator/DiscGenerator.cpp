// DiscGenerator.cpp
#include "DiscGenerator.h"
#include <cmath>

void DiscGenerator::generate(ParticleContainer& container) const {
  const double physicalRadius = static_cast<double>(radiusCells_) * h_;
  const double R2 = physicalRadius * physicalRadius;

  const std::size_t estimatedCount =
      static_cast<std::size_t>(3.14159 * radiusCells_ * radiusCells_);
  container.reserve(container.size() + estimatedCount);

  for (int i = -radiusCells_; i <= radiusCells_; ++i) {
    for (int j = -radiusCells_; j <= radiusCells_; ++j) {
      double dx = static_cast<double>(i) * h_;
      double dy = static_cast<double>(j) * h_;

      if (dx * dx + dy * dy <= R2) {
        std::array<double, 3> pos = {
          center_[0] + dx,
          center_[1] + dy,
          center_[2]      ,
      };

        std::array<double, 3> vel = baseVelocity_;

        container.emplaceParticle(pos, vel, mass_, type_);
      }
    }
  }
}
