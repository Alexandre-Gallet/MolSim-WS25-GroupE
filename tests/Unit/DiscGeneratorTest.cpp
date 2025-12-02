//
// Created by altay on 2.12.2025.
//

#include "DiscGeneratorTest.h"
#include <array>
#include <cmath>
#include "gtest/gtest.h"

#include "Generator/DiscGenerator.h"
#include "Generator/ParticleGenerator.h"
#include "Particle.h"
#include "ParticleContainer.h"


namespace {
/*
 * Toleranz für Gleitkomma-Vergleiche.
 */
constexpr double tolerance = 1e-12;

const std::array<double, 3> ZERO{0.0, 0.0, 0.0};
}


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

  // Es sollte genau ein Partikel geben.
  ASSERT_EQ(container.size(), 1u);

  const auto &p = *container.begin();
  const auto &x = p.getX();

  EXPECT_NEAR(x[0], center[0], tolerance);
  EXPECT_NEAR(x[1], center[1], tolerance);
  EXPECT_NEAR(x[2], center[2], tolerance);

  EXPECT_NEAR(p.getM(), mass, tolerance);
  EXPECT_EQ(p.getType(), type);

  const auto &v = p.getV();
  EXPECT_NEAR(v[0], baseVelocity[0], tolerance);
  EXPECT_NEAR(v[1], baseVelocity[1], tolerance);
  EXPECT_NEAR(v[2], baseVelocity[2], tolerance);
}


