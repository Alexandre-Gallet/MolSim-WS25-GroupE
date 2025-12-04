//
// Created by altay on 2.12.2025.
//

#include "DiscGeneratorTest.h"
#include <array>
#include <cmath>
#include "gtest/gtest.h"

#include "Generator/DiscGenerator.h"
#include "Generator/ParticleGenerator.h"
#include "../../src/Container/ParticleContainer.h"
#include "../../src/Container/Particle.h"
namespace {
constexpr double tolerance = 1e-12; // tolerance for floating point comparisons
const std::array<double, 3> ZERO{0.0, 0.0, 0.0};
}

// Test 1: radiusCells = 0 should generate exactly one particle
//         located exactly at the center.
TEST(DiscGeneratorTest, RadiusZeroProducesSingleParticleAtCenter) {
  ParticleContainer container;

  std::array<double, 3> center{1.0, 2.0, 0.0};
  const int radiusCells = 0;
  const double h = 1.0;
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};
  const int type = 1;

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass,
                              baseVelocity, type);

  // Expect exactly one particle
  ASSERT_EQ(container.size(), 1u);

  const auto &p = *container.begin();
  const auto &x = p.getX();

  // Position must match center exactly
  EXPECT_NEAR(x[0], center[0], tolerance);
  EXPECT_NEAR(x[1], center[1], tolerance);
  EXPECT_NEAR(x[2], center[2], tolerance);

  // Mass and type must be correct
  EXPECT_NEAR(p.getM(), mass, tolerance);
  EXPECT_EQ(p.getType(), type);

  // Velocity must equal baseVelocity
  const auto &v = p.getV();
  EXPECT_NEAR(v[0], baseVelocity[0], tolerance);
  EXPECT_NEAR(v[1], baseVelocity[1], tolerance);
  EXPECT_NEAR(v[2], baseVelocity[2], tolerance);
}

// Test 2: radiusCells = 1 with h = 1.0 should produce exactly
//         the 5 lattice points inside the circle:
//         (0,0), (1,0), (-1,0), (0,1), (0,-1)
TEST(DiscGeneratorBasicTest, RadiusOneHasFiveExpectedLatticePoints) {
  ParticleContainer container;

  std::array<double, 3> center{0.0, 0.0, 0.0};
  const int radiusCells = 1;
  const double h = 1.0;
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};
  const int type = 2;

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass,
                              baseVelocity, type);

  // Expect exactly 5 particles for radius 1
  ASSERT_EQ(container.size(), 5u);

  // Expected absolute positions
  const std::array<std::array<double, 3>, 5> expectedPositions = {{
    {center[0] + 0.0, center[1] + 0.0, center[2]},  // center
    {center[0] + 1.0, center[1] + 0.0, center[2]},  // right
    {center[0] - 1.0, center[1] + 0.0, center[2]},  // left
    {center[0] + 0.0, center[1] + 1.0, center[2]},  // up
    {center[0] + 0.0, center[1] - 1.0, center[2]},  // down
  }};

  // Check whether every expected point exists
  for (const auto &expected : expectedPositions) {
    bool found = false;

    for (auto &p : container) {
      const auto &x = p.getX();
      if (std::fabs(x[0] - expected[0]) < tolerance &&
          std::fabs(x[1] - expected[1]) < tolerance &&
          std::fabs(x[2] - expected[2]) < tolerance) {
        found = true;
        break;
      }
    }

    EXPECT_TRUE(found) << "Expected position ("
                       << expected[0] << ", " << expected[1] << ", " << expected[2]
                       << ") was not found in the generated disc.";
  }
}

// Test 3: Existing particles should not be removed or changed
//         when generateDisc() is called again.
TEST(DiscGeneratorBasicTest, ExistingParticlesArePreservedByGenerateDisc) {
  ParticleContainer container;

  // Marker particle with unique type and known state
  const int markerType = 99;
  std::array<double, 3> markerPos{5.0, 5.0, 0.0};
  std::array<double, 3> markerVel{0.1, 0.2, 0.3};
  const double markerMass = 2.0;

  container.emplaceParticle(markerPos, markerVel, markerMass, markerType);
  const std::size_t initialSize = container.size();

  // Add disc particles
  std::array<double, 3> center{0.0, 0.0, 0.0};
  const int radiusCells = 2;
  const double h = 1.0;
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};
  const int type = 1;

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass,
                              baseVelocity, type);

  // Container should now contain more particles
  ASSERT_GT(container.size(), initialSize);

  // Marker particle must still exist unchanged
  bool foundMarker = false;

  for (auto &p : container) {
    if (p.getType() == markerType) {
      const auto &x = p.getX();
      const auto &v = p.getV();

      if (std::fabs(x[0] - markerPos[0]) < tolerance &&
          std::fabs(x[1] - markerPos[1]) < tolerance &&
          std::fabs(x[2] - markerPos[2]) < tolerance &&
          std::fabs(v[0] - markerVel[0]) < tolerance &&
          std::fabs(v[1] - markerVel[1]) < tolerance &&
          std::fabs(v[2] - markerVel[2]) < tolerance) {
        foundMarker = true;
        break;
      }
    }
  }

  EXPECT_TRUE(foundMarker)
      << "The existing marker particle was removed or modified by generateDisc().";
}

// Test 4: All generated particles must lie inside the physical
//         disc radius and exactly on the discretization grid.
TEST(DiscGeneratorBasicTest, AllParticlesInsideAndOnGrid) {
  ParticleContainer container;

  std::array<double, 3> center{1.0, 2.0, 0.0};
  const int radiusCells = 3;
  const double h = 0.5;
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};
  const int type = 2;

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass,
                              baseVelocity, type);

  const double R = h * radiusCells;   // physical radius
  const double R2 = R * R;            // squared radius

  for (const auto &p : container) {
    const auto &x = p.getX();

    double dx = x[0] - center[0];
    double dy = x[1] - center[1];

    // Particle must lie inside (or on) the disc
    EXPECT_LE(dx*dx + dy*dy, R2 + tolerance);

    // Must be aligned on the h-grid
    EXPECT_NEAR(std::round(dx / h), dx / h, 1e-9);
    EXPECT_NEAR(std::round(dy / h), dy / h, 1e-9);

    // z-coordinate should match the center since this is a 2D disc
    EXPECT_NEAR(x[2], center[2], tolerance);
  }
}
