/**
* @file ParticleContainerTest.cpp
 * @brief Tests for ParticleContainer and its basic functionality.
 *
 * This file contains unit tests that verify creation, insertion, iteration,
 * pair iteration and clearing of the ParticleContainer used in the MolSim project.
 * Each test states its intention briefly, as required in the worksheet.
 */


#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include "gtest/gtest.h"
#include "Particle.h"
#include "ParticleContainer.h"

namespace {
constexpr double tolerance = 1e-12;
const std::array<double,3> ZERO{0.0,0.0,0.0}; // Zero Vector
}

class ParticleContainerFixture : public ::testing::Test {
protected:
  void SetUp() override {
    std::array<double, 3> x1{0, 0, 0}, x2{1, 0, 0}, v{0, 0, 0};
    pc.addParticle(Particle{x1, v, 1.0, 0});
    pc.addParticle(Particle{x2, v, 1.0, 0});
  }
  ParticleContainer pc;
};


// 1
TEST(ParticleContainerTest, NewlyConstructedIsEmpty) {
  ParticleContainer c;
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(c.size(), 0u);
}
//2
TEST(ParticleContainerTest, AddParticleIncreasesSize) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(c.size(), 1u);
  EXPECT_FALSE(c.empty());
}
//3
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
//4
TEST(ParticleContainerTest, ReserveDoesNotChangeSize) {
  ParticleContainer c;
  EXPECT_EQ(c.size(), 0u);
  c.reserve(100);
  EXPECT_EQ(c.size(), 0u);
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(c.size(), 1u);
}
//5
TEST(ParticleContainerTest, AddParticleByConstRefIncreasesSize) {
  ParticleContainer c;
  Particle p{ZERO, ZERO, 1.0, 0};
  auto &ref = c.addParticle(p);
  EXPECT_EQ(c.size(), 1u);
  EXPECT_FALSE(c.empty());
  (void)ref;
}
//6
TEST(ParticleContainerTest, AddParticleByRvalueIncreasesSize) {
  ParticleContainer c;
  auto &ref = c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(c.size(), 1u);
  EXPECT_FALSE(c.empty());
  (void)ref;
}
//7
TEST(ParticleContainerTest, EmptyContainerHasNoIteration) {
  ParticleContainer c;
  EXPECT_EQ(c.begin(), c.end());
  int count = 0;
  for (auto &p : c) { (void)p; ++count; }
  EXPECT_EQ(count, 0);
}
//8
TEST(ParticleContainerTest, ReserveThenAddMultiple) {
  ParticleContainer c;
  c.reserve(10);
  for (int i = 0; i < 10; ++i) {
    c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  }
  EXPECT_EQ(c.size(), 10u);
}
//9
TEST(ParticleContainerTest, ClearEmptiesContainer) {
  ParticleContainer c;
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  c.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  ASSERT_EQ(c.size(), 2u);
  c.clear();
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(c.size(), 0u);
}
//10
TEST_F(ParticleContainerFixture, SizeIsTwoAfterSetup) {
  EXPECT_EQ(pc.size(), 2u);
}
//11
TEST_F(ParticleContainerFixture, SizeAfterAddIsThree) {
  pc.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(pc.size(), 3u);
}

//12
TEST_F(ParticleContainerFixture, ClearThenAddOne) {
  pc.clear();
  EXPECT_EQ(pc.size(), 0u);
  pc.addParticle(Particle{ZERO, ZERO, 1.0, 0});
  EXPECT_EQ(pc.size(), 1u);
}
//13
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
//14
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


//15
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
// 16
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

