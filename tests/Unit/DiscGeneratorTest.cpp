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

/*
 * Test 2:
 * Radius = 1 und Gitterabstand h = 1.0.
 *
 * Die Diskretisierung sollte dann folgende Punkte enthalten:
 *   (0,0), (1,0), (-1,0), (0,1), (0,-1) relativ zum Zentrum.
 *
 * Wir überprüfen:
 *   - Anzahl der Partikel ist genau 5.
 *   - Alle fünf erwarteten Positionen sind vorhanden.
 */
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

  // Für radiusCells = 1 und h = 1 sollten genau 5 Gitterpunkte im Kreis liegen.
  ASSERT_EQ(container.size(), 5u);

  // Erwartete Positionen relativ zum Zentrum.
  const std::array<std::array<double, 3>, 5> expectedPositions = {{
    {center[0] + 0.0, center[1] + 0.0, center[2]},  // Zentrum
    {center[0] + 1.0, center[1] + 0.0, center[2]},  // rechts
    {center[0] - 1.0, center[1] + 0.0, center[2]},  // links
    {center[0] + 0.0, center[1] + 1.0, center[2]},  // oben
    {center[0] + 0.0, center[1] - 1.0, center[2]},  // unten
}};

  // Für jede erwartete Position prüfen wir, ob ein Partikel in der Nähe existiert.
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
    EXPECT_TRUE(found) << "Erwartete Position ("
                       << expected[0] << ", " << expected[1] << ", " << expected[2]
                       << ") wurde nicht gefunden.";
  }
}

TEST(DiscGeneratorBasicTest, ExistingParticlesArePreservedByGenerateDisc) {
  ParticleContainer container;

  // Marker-Partikel mit eindeutigem Typ.
  const int markerType = 99;
  std::array<double, 3> markerPos{5.0, 5.0, 0.0};
  std::array<double, 3> markerVel{0.1, 0.2, 0.3};
  const double markerMass = 2.0;

  container.emplaceParticle(markerPos, markerVel, markerMass, markerType);
  const std::size_t initialSize = container.size();

  // Nun eine Disk hinzufügen.
  std::array<double, 3> center{0.0, 0.0, 0.0};
  const int radiusCells = 2;
  const double h = 1.0;
  const double mass = 1.0;
  const std::array<double, 3> baseVelocity{0.0, 0.0, 0.0};
  const int type = 1;

  DiscGenerator::generateDisc(container, center, radiusCells, h, mass,
                              baseVelocity, type);

  // Container sollte jetzt mehr Partikel enthalten als vorher.
  ASSERT_GT(container.size(), initialSize);

  // Marker-Partikel sollte noch existieren (Typ + Position + Geschwindigkeit).
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
      << "Der vorhandene Marker-Partikel wurde durch generateDisc() entfernt.";
}


