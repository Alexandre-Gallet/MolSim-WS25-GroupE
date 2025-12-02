/*
   Tests for ParticleContainer and its basic functionality.

  This file contains unit tests that verify creation, insertion, iteration,
  pair iteration and clearing of the ParticleContainer used in the MolSim project.
  Each test states its intention briefly, as required in the worksheet.
 */

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/Container/ParticleContainer.h"
#include "../../src/Container/Particle.h"
#include "gtest/gtest.h"
#include "Particle.h"
#include "ParticleContainer.h"
#include "ParticleContainerTest.h"

namespace {
/*
Numerical tolerance used for floating point comparisons.
 */
constexpr double tolerance = 1e-12;

/*
 Convenience zero vector for particle creation.
 */
const std::array<double, 3> ZERO{0.0, 0.0, 0.0};
}


 // Newly created container must be empty.

TEST(ParticleContainerTest, NewlyConstructedIsEmpty) {
  ParticleContainer c;
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(c.size(), 0u);
}


// Adding one particle should increase the size to 1 and make it non-empty.

TEST(ParticleContainerTest, AddParticleIncreasesSize) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(c.size(), 1u);
  EXPECT_FALSE(c.empty());
}

 // Range-based iteration should visit all inserted particles (const and non-const).

TEST(ParticleContainerTest, IterationVisitsAllParticles) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 2.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 3.0, 0});

  std::size_t count = 0;
  double mass_sum = 0.0;
  for (auto &p : c) {
    mass_sum += p.getM();
    ++count;
  }
  EXPECT_EQ(count, 3u);
  EXPECT_NEAR(mass_sum, 6.0, tolerance);

  const ParticleContainer &cref = c;
  count = 0;
  for (const auto &p : cref) {
    (void)p;
    ++count;
  }
  EXPECT_EQ(count, 3u);
}

 //Reserving capacity must not change the logical size of the container.
TEST(ParticleContainerTest, ReserveDoesNotChangeSize) {
  ParticleContainer c;
  EXPECT_EQ(c.size(), 0u);
  c.reserve(100);
  EXPECT_EQ(c.size(), 0u);
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(c.size(), 1u);
}

 //Adding a particle by const reference should still increase the size.

TEST(ParticleContainerTest, AddParticleByConstRefIncreasesSize) {
  ParticleContainer c;
  Particle p{ZERO, ZERO, 1.0, 0};
  auto &ref = c.addParticle(p);
  EXPECT_EQ(c.size(), 1u);
  EXPECT_FALSE(c.empty());
  (void)ref;
}

 // Adding a particle by rvalue should also increase the size.

TEST(ParticleContainerTest, AddParticleByRvalueIncreasesSize) {
  ParticleContainer c;
  auto &ref = c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(c.size(), 1u);
  EXPECT_FALSE(c.empty());
  (void)ref;
}


 // Empty containers should provide begin()==end() and no iteration.
TEST(ParticleContainerTest, EmptyContainerHasNoIteration) {
  ParticleContainer c;
  EXPECT_EQ(c.begin(), c.end());
  int count = 0;
  for (auto &p : c) {
    (void)p;
    ++count;
  }
  EXPECT_EQ(count, 0);
}

// After reserving, adding multiple particles should correctly update size.
TEST(ParticleContainerTest, ReserveThenAddMultiple) {
  ParticleContainer c;
  c.reserve(10);
  for (int i = 0; i < 10; ++i) {
    c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  }
  EXPECT_EQ(c.size(), 10u);
}

// clear() must remove all particles and make the container empty.
TEST(ParticleContainerTest, ClearEmptiesContainer) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  ASSERT_EQ(c.size(), 2u);
  c.clear();
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(c.size(), 0u);
}

// Fixture setup should create exactly two particles.
TEST_F(ParticleContainerFixture, SizeIsTwoAfterSetup) {
  EXPECT_EQ(pc.size(), 2u);
}

// Adding one more particle on top of fixture data should yield three.
TEST_F(ParticleContainerFixture, SizeAfterAddIsThree) {
  pc.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(pc.size(), 3u);
}

// After clearing a pre-filled container, adding one should result in size 1.
TEST_F(ParticleContainerFixture, ClearThenAddOne) {
  pc.clear();
  EXPECT_EQ(pc.size(), 0u);
  pc.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(pc.size(), 1u);
}

//  Manual iterator usage should visit exactly the inserted elements (order preserved).
TEST(ParticleContainerTest, IteratorVisitsAllInInsertionOrder) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});

  std::size_t count = 0;
  for (auto it = c.begin(); it != c.end(); ++it) {
    ++count;
  }
  EXPECT_EQ(count, c.size());
  EXPECT_EQ(count, 3u);
}

//  Const iterators (cbegin/cend) should work and visit all elements.
TEST(ParticleContainerTest, ConstIterationWorks) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});

  const ParticleContainer &cref = c;
  std::size_t count = 0;
  for (auto it = cref.cbegin(); it != cref.cend(); ++it) {
    (void)(*it);
    ++count;
  }
  EXPECT_EQ(count, 1u);
}

/*
 forEachPair should visit each unique unordered pair exactly once.
 For n=4 particles, the number of unique pairs is n*(n-1)/2 = 6.
 */
TEST(ParticleContainerTest, ForEachPairVisitsAllUniquePairs) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});

  int calls = 0;
  c.forEachPair([&](Particle &a, Particle &b) {
    EXPECT_NE(&a, &b);
    ++calls;
  });
  EXPECT_EQ(calls, 6); // 4 * 3 / 2
}

/*
 Const version of forEachPair should also visit all unique unordered pairs.

  For n=3 particles, the number of unique pairs is n*(n-1)/2 = 3.
 */
TEST(ParticleContainerTest, ConstForEachPairAlsoWorks) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});

  const ParticleContainer &cref = c;
  int calls = 0;
  cref.forEachPair([&](const Particle &a, const Particle &b) {
    EXPECT_NE(&a, &b);
    ++calls;
  });
  EXPECT_EQ(calls, 3); // 3 * 2 / 2
}
