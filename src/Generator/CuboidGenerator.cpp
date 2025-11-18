#include "CuboidGenerator.h"

#include "../utils/MaxwellBoltzmannDistribution.h"

void CuboidGenerator::generateCuboid(ParticleContainer &container, const std::array<double, 3> &origin,
                                     const std::array<size_t, 3> &numPerDim, double h, double mass,
                                     const std::array<double, 3> &baseVelocity, double brownianMean, int type) {
  const std::size_t total = numPerDim[0] * numPerDim[1] * numPerDim[2];
  container.reserve(container.size() + total);

  for (std::size_t i = 0; i < numPerDim[0]; ++i) {
    for (std::size_t j = 0; j < numPerDim[1]; ++j) {
      for (std::size_t k = 0; k < numPerDim[2]; ++k) {
        std::array<double, 3> pos = {
          origin[0] + i * h,
          origin[1] + j * h,
          origin[2] + k * h
      };

        std::array<double, 3> vel = baseVelocity;

        // 3 for the Dimension
        auto brownian = maxwellBoltzmannDistributedVelocity(brownianMean, 3);

        vel[0] += brownian[0];
        vel[1] += brownian[1];
        vel[2] += brownian[2];

        container.emplaceParticle(pos, vel, mass, type);
      }
    }
  }
}
