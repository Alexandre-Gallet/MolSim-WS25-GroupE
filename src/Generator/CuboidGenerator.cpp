// CuboidGenerator.cpp
#include "CuboidGenerator.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void CuboidGenerator::generate(Container &container) const {
  // Simply call the static helper with the stored parameters
  generateCuboid(container, origin_, numPerDim_, dom_size_, h_, mass_, baseVelocity_, brownianMean_, type_);
}
// Have kept the generateCuboid function due to code structure, but also kept generate function as a design decision
void CuboidGenerator::generateCuboid(Container &container, const std::array<double, 3> &origin,
                                     const std::array<int, 3> &numPerDim, const std::array<double, 3> &dom_size,
                                     double h, double mass, const std::array<double, 3> &baseVelocity,
                                     double brownianMean, int type) {
  const int total = numPerDim[0] * numPerDim[1] * numPerDim[2];
  container.reserve(container.size() + total);

  for (int i = 0; i < numPerDim[0]; ++i) {
    for (int j = 0; j < numPerDim[1]; ++j) {
      for (int k = 0; k < numPerDim[2]; ++k) {
        std::array<double, 3> pos = {origin[0] + i * h, origin[1] + j * h, origin[2] + k * h};

        std::array<double, 3> vel = baseVelocity;

        // 3 for the dimension
        auto brownian = maxwellBoltzmannDistributedVelocity(brownianMean, 3);

        vel[0] += brownian[0];
        vel[1] += brownian[1];
        // the simulation still doesn't work for 3D input, we're working on fixing this condition
        if (dom_size[2] > 1.) {
          // vel[2] += brownian[2];
        }

        container.emplaceParticle(pos, vel, mass, type);
      }
    }
  }
}
