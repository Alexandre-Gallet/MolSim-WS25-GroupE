#include <Container/ParticleContainer.h>

#include "Container/Container.h"
#include "Generator/DiscGenerator.h"
#include "gtest/gtest.h"

// TEST 1;
// DiscGenerator + ParticleContainer should produce a dense region of particles
// where at least some neighbor interactions (within cutoff) exist.
TEST(DiscGeneratorBehaviourTest, DiscProducesNeighborsWithinCutoff) {
  ParticleContainer container;

  std::array<double, 3> center{0.0, 0.0, 0.0};
  const int radiusCells = 2;
  const double h = 1.0;
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};
  const int type = 1;

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass, baseVelocity, type);

  ASSERT_GT(container.size(), 1u);  // must contain multiple particles

  const double cutoff = 1.5;
  const double cutoff2 = cutoff * cutoff;

  int neighborInteractions = 0;

  int i = 0;
  for (auto it1 = container.begin(); it1 != container.end(); ++it1, ++i) {
    int j = i + 1;
    auto it2 = it1;
    ++it2;

    for (; it2 != container.end(); ++it2, ++j) {
      Particle &p1 = *it1;
      Particle &p2 = *it2;

      const auto &xA = p1.getX();
      const auto &xB = p2.getX();

      double dx = xA[0] - xB[0];
      double dy = xA[1] - xB[1];
      double dz = xA[2] - xB[2];

      double r2 = dx * dx + dy * dy + dz * dz;

      if (r2 < cutoff2) {
        ++neighborInteractions;
      }
    }
  }

  // For a reasonably filled disc, there should be at least one neighbor interaction.
  EXPECT_GE(neighborInteractions, 1);
}
