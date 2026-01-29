#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <vector>

#include "Container/ParticleContainer.h"
#include "ForceCalculation/MembraneForce.h"
#include "Generator/MembraneGenerator.h"
#include "inputReader/SimulationConfig.h"

// Helper to look up the rest length of a specific neighbor edge.
double restLengthTo(const Particle &p, const Particle *target) {
  for (const auto &n : p.getNeighbors()) {
    if (n.particle == target) {
      return n.rest_length;
    }
  }
  return -1.0;
}

// Builds a tiny 2x2x1 membrane and checks lattice placement plus symmetric neighbor wiring.
TEST(MembraneGeneratorTest, CreatesGridAndNeighbors) {
  ParticleContainer container;
  MembraneConfig cfg;
  cfg.origin = {0.0, 0.0, 0.0};
  cfg.n = {2, 2, 1};
  cfg.h = 1.0;
  cfg.r0 = 1.0;

  auto grid = MembraneGenerator::generate(container, cfg);

  ASSERT_EQ(grid.size(), 4u);
  for (auto *p : grid) {
    ASSERT_NE(p, nullptr);
  }

  auto *p00 = MembraneGenerator::particleAt(grid, cfg.n, 0, 0, 0);
  auto *p10 = MembraneGenerator::particleAt(grid, cfg.n, 1, 0, 0);
  auto *p01 = MembraneGenerator::particleAt(grid, cfg.n, 0, 1, 0);
  auto *p11 = MembraneGenerator::particleAt(grid, cfg.n, 1, 1, 0);

  ASSERT_NE(p00, nullptr);
  ASSERT_NE(p10, nullptr);
  ASSERT_NE(p01, nullptr);
  ASSERT_NE(p11, nullptr);

  // Each grid node should sit at origin + index * h.
  EXPECT_DOUBLE_EQ(p00->getX()[0], 0.0);
  EXPECT_DOUBLE_EQ(p00->getX()[1], 0.0);
  EXPECT_DOUBLE_EQ(p10->getX()[0], 1.0);
  EXPECT_DOUBLE_EQ(p10->getX()[1], 0.0);
  EXPECT_DOUBLE_EQ(p01->getX()[0], 0.0);
  EXPECT_DOUBLE_EQ(p01->getX()[1], 1.0);
  EXPECT_DOUBLE_EQ(p11->getX()[0], 1.0);
  EXPECT_DOUBLE_EQ(p11->getX()[1], 1.0);

  // Corner node connects to +x, +y, and in-plane diagonal.
  ASSERT_EQ(p00->getNeighbors().size(), 3u);
  EXPECT_NEAR(restLengthTo(*p00, p10), cfg.r0, 1e-12);
  EXPECT_NEAR(restLengthTo(*p00, p01), cfg.r0, 1e-12);
  EXPECT_NEAR(restLengthTo(*p00, p11), std::sqrt(2.0) * cfg.r0, 1e-12);

  // Every connection is stored symmetrically with the same rest length.
  EXPECT_NEAR(restLengthTo(*p10, p00), cfg.r0, 1e-12);
  EXPECT_NEAR(restLengthTo(*p01, p00), cfg.r0, 1e-12);
  EXPECT_NEAR(restLengthTo(*p11, p00), std::sqrt(2.0) * cfg.r0, 1e-12);
}

// Verifies gravity + pull stacking and that the previous force is preserved in old_f.
TEST(MembraneForceTest, AppliesGravityAndPull) {
  ParticleContainer container;
  auto &p = container.emplaceParticle(std::array<double, 3>{0.0, 0.0, 0.0}, std::array<double, 3>{0.0, 0.0, 0.0}, 2.0, 0);

  MembraneForce force(/*epsilon*/ 0.0, /*sigma*/ 1.0, /*k*/ 0.0, /*r0*/ 1.0, /*pull_force*/ 5.0, /*pull_until*/ 10.0,
                      std::array<double, 3>{0.0, -9.81, 0.0});
  force.setPullTargets({&p});
  force.setCurrentTime(1.0);

  p.setF({1.0, 1.0, 1.0});  // Non-zero start to ensure oldF capture works.
  force.calculateF(container);

  auto f = p.getF();
  EXPECT_NEAR(p.getOldF()[0], 1.0, 1e-12);
  EXPECT_NEAR(f[0], 0.0, 1e-12);
  EXPECT_NEAR(f[1], -19.62, 1e-12);  // m * g
  EXPECT_NEAR(f[2], 5.0, 1e-12);     // upward pull
}

// Stretched spring along x-axis should yield equal and opposite Hooke forces.
TEST(MembraneForceTest, SpringForceIsSymmetric) {
  ParticleContainer container;
  container.reserve(2);
  auto &p1 = container.emplaceParticle(std::array<double, 3>{0.0, 0.0, 0.0}, std::array<double, 3>{0.0, 0.0, 0.0}, 1.0, 0);
  auto &p2 = container.emplaceParticle(std::array<double, 3>{2.0, 0.0, 0.0}, std::array<double, 3>{0.0, 0.0, 0.0}, 1.0, 0);

  p1.clearNeighbors();
  p2.clearNeighbors();
  p1.addNeighbor(&p2, 1.0);
  p2.addNeighbor(&p1, 1.0);

  ASSERT_EQ(p1.getNeighbors().size(), 1u);
  ASSERT_EQ(p2.getNeighbors().size(), 1u);

  MembraneForce force(/*epsilon*/ 0.0, /*sigma*/ 1.0, /*k*/ 10.0, /*r0*/ 1.0, /*pull_force*/ 0.0, /*pull_until*/ 0.0,
                      std::array<double, 3>{0.0, 0.0, 0.0});
  force.calculateF(container);

  auto f1 = p1.getF();
  auto f2 = p2.getF();
  EXPECT_NEAR(f1[0], 10.0, 1e-12);
  EXPECT_NEAR(f2[0], -10.0, 1e-12);
  EXPECT_DOUBLE_EQ(f1[1], 0.0);
  EXPECT_DOUBLE_EQ(f1[2], 0.0);
}

// Repulsive LJ component should push particles apart when they are inside the cutoff.
TEST(MembraneForceTest, RepulsiveWithinCutoff) {
  ParticleContainer container;
  container.reserve(2);
  auto &p1 = container.emplaceParticle(std::array<double, 3>{0.0, 0.0, 0.0}, std::array<double, 3>{0.0, 0.0, 0.0}, 1.0, 0);
  auto &p2 = container.emplaceParticle(std::array<double, 3>{1.0, 0.0, 0.0}, std::array<double, 3>{0.0, 0.0, 0.0}, 1.0, 0);

  MembraneForce force(/*epsilon*/ 1.0, /*sigma*/ 1.0, /*k*/ 0.0, /*r0*/ 1.0, /*pull_force*/ 0.0, /*pull_until*/ 0.0,
                      std::array<double, 3>{0.0, 0.0, 0.0});
  force.calculateF(container);

  auto f1 = p1.getF();
  auto f2 = p2.getF();
  const std::array<double, 3> r12{p2.getX()[0] - p1.getX()[0], p2.getX()[1] - p1.getX()[1], p2.getX()[2] - p1.getX()[2]};
  const double dot = f1[0] * r12[0] + f1[1] * r12[1] + f1[2] * r12[2];

  EXPECT_LT(dot, 0.0);  // pushing apart along the separation vector
  EXPECT_NEAR(f1[0] + f2[0], 0.0, 1e-10);
  EXPECT_NEAR(f1[1] + f2[1], 0.0, 1e-10);
  EXPECT_NEAR(f1[2] + f2[2], 0.0, 1e-10);
}
