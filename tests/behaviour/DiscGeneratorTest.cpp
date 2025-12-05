#include <Container/ParticleContainer.h>

#include "Container/Container.h"
#include "Generator/DiscGenerator.h"
#include "gtest/gtest.h"
#include <cmath>


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

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass,
                              baseVelocity, type);

  ASSERT_GT(container.size(), 1u); // must contain multiple particles

  const double cutoff = 1.5;
  const double cutoff2 = cutoff * cutoff;

  int neighborInteractions = 0;

  int i = 0;
  for (auto it1 = container.begin(); it1 != container.end(); ++it1, ++i) {
    int j = i + 1;
    auto it2 = it1;
    ++it2;

    for (; it2 != container.end(); ++it2, ++j) {
      Particle& p1 = *it1;
      Particle& p2 = *it2;

      const auto& xA = p1.getX();
      const auto& xB = p2.getX();

      double dx = xA[0] - xB[0];
      double dy = xA[1] - xB[1];
      double dz = xA[2] - xB[2];

      double r2 = dx*dx + dy*dy + dz*dz;

      if (r2 < cutoff2) {
        ++neighborInteractions;
      }
    }
  }

  // For a reasonably filled disc, there should be at least one neighbor interaction.
  EXPECT_GE(neighborInteractions, 1);
}
TEST(FallingDropBehaviourTest, DiscRespectsDomainAndInitialVelocity) {
  ParticleContainer container;

  const std::array<double, 3> center{60.0, 25.0, 0.0};
  const int radiusCells = 15;
  const double h = std::pow(2.0, 1.0 / 6.0);
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, -10.0, 0.0};
  const int type = 1;
  const std::array<double, 3> domain_size{120.0, 50.0, 1.0};

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass, baseVelocity, type);

  // Compute expected lattice count by scanning the disc grid.
  std::size_t expectedCount = 0;
  const double R2 = std::pow(radiusCells * h, 2);
  for (int i = -radiusCells; i <= radiusCells; ++i) {
    for (int j = -radiusCells; j <= radiusCells; ++j) {
      const double dx = i * h;
      const double dy = j * h;
      if (dx * dx + dy * dy <= R2) {
        ++expectedCount;
      }
    }
  }
  ASSERT_EQ(container.size(), expectedCount);

  std::array<double, 3> minPos{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                               std::numeric_limits<double>::max()};
  std::array<double, 3> maxPos{std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(),
                               std::numeric_limits<double>::lowest()};

  for (auto &p : container) {
    const auto &x = p.getX();
    const auto &v = p.getV();

    minPos[0] = std::min(minPos[0], x[0]);
    minPos[1] = std::min(minPos[1], x[1]);
    minPos[2] = std::min(minPos[2], x[2]);

    maxPos[0] = std::max(maxPos[0], x[0]);
    maxPos[1] = std::max(maxPos[1], x[1]);
    maxPos[2] = std::max(maxPos[2], x[2]);

    EXPECT_DOUBLE_EQ(p.getM(), mass);
    EXPECT_EQ(p.getType(), type);

    EXPECT_DOUBLE_EQ(v[0], baseVelocity[0]);
    EXPECT_DOUBLE_EQ(v[1], baseVelocity[1]);
    EXPECT_DOUBLE_EQ(v[2], baseVelocity[2]);
  }

  EXPECT_GE(minPos[0], 0.0);
  EXPECT_LE(maxPos[0], domain_size[0]);
  EXPECT_GE(minPos[1], 0.0);
  EXPECT_LE(maxPos[1], domain_size[1]);

  EXPECT_NEAR(minPos[2], center[2], 1e-12);
  EXPECT_NEAR(maxPos[2], center[2], 1e-12);
}

// Task 4 falling-drop: neighbours should sit on the h-grid with minimum spacing ~h.
TEST(FallingDropBehaviourTest, DiscProducesRegularSpacing) {
  ParticleContainer container;

  const double h = std::pow(2.0, 1.0 / 6.0);
  DiscGenerator::generateDisc(container, {0.0, 0.0, 0.0}, 6, h, 1.0, {0.0, 0.0, 0.0}, 3);

  ASSERT_GT(container.size(), 1u);

  double minDistance = std::numeric_limits<double>::max();
  for (auto it1 = container.begin(); it1 != container.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    for (; it2 != container.end(); ++it2) {
      const auto &a = it1->getX();
      const auto &b = it2->getX();
      const double dx = a[0] - b[0];
      const double dy = a[1] - b[1];
      const double dz = a[2] - b[2];
      const double dist = std::sqrt(dx * dx + dy * dy + dz * dz);
      minDistance = std::min(minDistance, dist);
    }
  }

  EXPECT_NEAR(minDistance, h, 1e-9);
}
