#include <gtest/gtest.h>

#include <array>
#include <algorithm>
#include <set>
#include <utility>
#include <vector>

#include "../../src/Container/LinkedCellContainer.h"
#include "../../src/Container/Particle.h"

namespace {
// Store pairs with deterministic ordering to simplify lookups.
std::pair<const Particle*, const Particle*> makeOrderedPair(Particle &a, Particle &b) {
  if (&a > &b) {
    return {&b, &a};
  }
  return {&a, &b};
}
}  // namespace

TEST(LinkedCellContainerTest, ForEachPairVisitsCurrentAndNeighborCellsOnly) {
  // Domain yields 4x4x1 inner grid (padded 6x6x3). Center cell has index (2,2,1).
  LinkedCellContainer container(1.0, {4.0, 4.0, 1.0});

  auto &a = container.emplaceParticle({1.1, 1.1, 0.2}, {0, 0, 0}, 1.0);  // center (2,2,1)
  auto &b = container.emplaceParticle({1.4, 1.2, 0.2}, {0, 0, 0}, 1.0);  // center (2,2,1)
  auto &c = container.emplaceParticle({2.2, 1.1, 0.2}, {0, 0, 0}, 1.0);  // neighbor +x (3,2,1)
  // Far in +y direction (index difference 2): should not be paired with others.
  auto &far = container.emplaceParticle({1.1, 3.6, 0.2}, {0, 0, 0}, 1.0);  // (2,4,1)

  std::set<std::pair<const Particle*, const Particle*>> visited_pairs;
  container.forEachPair([&](Particle &p, Particle &q) {
    visited_pairs.insert(makeOrderedPair(p, q));
  });

  std::set<std::pair<const Particle*, const Particle*>> expected{
      makeOrderedPair(a, b), makeOrderedPair(a, c), makeOrderedPair(b, c)};

  EXPECT_EQ(visited_pairs, expected);
  EXPECT_EQ(visited_pairs.count(makeOrderedPair(a, far)), 0u);
  EXPECT_EQ(visited_pairs.count(makeOrderedPair(b, far)), 0u);
  EXPECT_EQ(visited_pairs.count(makeOrderedPair(c, far)), 0u);
}

TEST(LinkedCellContainerTest, ForEachPairDoesNotDuplicatePairs) {
  LinkedCellContainer container(1.0, {4.0, 4.0, 1.0});

  auto &a = container.emplaceParticle({1.1, 1.1, 0.2}, {0, 0, 0}, 1.0);
  auto &b = container.emplaceParticle({1.4, 1.2, 0.2}, {0, 0, 0}, 1.0);
  auto &c = container.emplaceParticle({2.2, 1.1, 0.2}, {0, 0, 0}, 1.0);
  auto &d = container.emplaceParticle({2.3, 1.8, 0.2}, {0, 0, 0}, 1.0);  // neighbor cell (3,3,1)

  std::vector<std::pair<const Particle*, const Particle*>> all_pairs;
  std::set<std::pair<const Particle*, const Particle*>> unique_pairs;

  container.forEachPair([&](Particle &p, Particle &q) {
    auto ordered = makeOrderedPair(p, q);
    all_pairs.push_back(ordered);
    unique_pairs.insert(ordered);
  });

  EXPECT_EQ(all_pairs.size(), unique_pairs.size());
}

TEST(LinkedCellContainerTest, BoundaryAndHaloIterationVisitCorrectParticles) {
  // Domain 3x3x3 -> padded 5x5x5; inner cell index (2,2,2), boundary when index is 1 or 3.
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});

  auto &inner = container.emplaceParticle({1.5, 1.5, 1.5}, {0, 0, 0}, 1.0);  // (2,2,2) inner
  auto &boundary = container.emplaceParticle({0.2, 1.5, 1.5}, {0, 0, 0}, 1.0);  // (1,2,2) boundary
  auto &halo = container.emplaceParticle({-0.1, 1.5, 1.5}, {0, 0, 0}, 1.0);      // (0,2,2) halo

  std::vector<const Particle*> boundary_seen;
  container.forEachBoundaryParticle([&](Particle *p) { boundary_seen.push_back(p); });
  EXPECT_EQ(boundary_seen.size(), 1u);
  EXPECT_EQ(boundary_seen.front(), &boundary);

  std::vector<const Particle*> halo_seen;
  container.forEachHaloParticle([&](Particle *p) { halo_seen.push_back(p); });
  EXPECT_EQ(halo_seen.size(), 1u);
  EXPECT_EQ(halo_seen.front(), &halo);

  // The inner particle should not be part of boundary or halo iteration.
  EXPECT_EQ(std::count(boundary_seen.begin(), boundary_seen.end(), &inner), 0);
  EXPECT_EQ(std::count(halo_seen.begin(), halo_seen.end(), &inner), 0);
}

TEST(LinkedCellContainerTest, FarApartCellsAreNotVisitedByForEachPair) {
  LinkedCellContainer container(1.0, {4.0, 4.0, 1.0});

  auto &left = container.emplaceParticle({1.1, 1.1, 0.2}, {0, 0, 0}, 1.0);   // (2,2,1)
  auto &right = container.emplaceParticle({3.6, 1.1, 0.2}, {0, 0, 0}, 1.0);  // (4,2,1) boundary, dx=2

  bool visited_far_pair = false;
  container.forEachPair([&](Particle &p, Particle &q) {
    auto ordered = makeOrderedPair(p, q);
    if (ordered == makeOrderedPair(left, right)) {
      visited_far_pair = true;
    }
  });

  EXPECT_FALSE(visited_far_pair);
}

TEST(LinkedCellContainerTest, OutflowRemovesHaloParticlesOnRebuild) {
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});

  container.emplaceParticle({3.2, 1.5, 1.5}, {0, 0, 0}, 1.0);  // outside +x halo
  container.emplaceParticle({1.0, 1.0, 1.0}, {0, 0, 0}, 1.0);  // inside domain

  container.rebuild();

  ASSERT_EQ(container.size(), 1u);
  const auto &remaining = *container.begin();
  EXPECT_DOUBLE_EQ(remaining.getX().at(0), 1.0);
}

TEST(LinkedCellContainerTest, ReflectingFaceCreatesGhostAndFlushesOnNextRebuild) {
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});
  std::array<BoundaryCondition, 6> bc{};
  bc.fill(BoundaryCondition::Outflow);
  bc[static_cast<std::size_t>(Face::XMin)] = BoundaryCondition::Reflecting;
  container.setBoundaryConditions(bc);

  auto &original =
      container.emplaceParticle({0.2, 1.5, 1.5}, {1.0, -0.5, 0.25}, 1.0);  // boundary on XMin

  container.rebuild();

  ASSERT_EQ(container.size(), 1u);  // ghosts live in the halo, not owned storage
  int ghosts_on_negative_x = 0;
  container.forEachHaloParticle([&](Particle *p) {
    const auto &pos = p->getX();
    const auto &vel = p->getV();
    ghosts_on_negative_x += pos[0] < 0.0 ? 1 : 0;
    if (pos[0] < 0.0) {
      EXPECT_DOUBLE_EQ(pos[0], -original.getX()[0]);
      EXPECT_DOUBLE_EQ(pos[1], original.getX()[1]);
      EXPECT_DOUBLE_EQ(pos[2], original.getX()[2]);
      EXPECT_DOUBLE_EQ(vel[0], -original.getV()[0]);
      EXPECT_DOUBLE_EQ(vel[1], original.getV()[1]);
      EXPECT_DOUBLE_EQ(vel[2], original.getV()[2]);
    }
  });
  EXPECT_EQ(ghosts_on_negative_x, 1);

  bc[static_cast<std::size_t>(Face::XMin)] = BoundaryCondition::Outflow;
  container.setBoundaryConditions(bc);
  container.rebuild();

  EXPECT_EQ(container.size(), 1u);
  EXPECT_DOUBLE_EQ(container.begin()->getX()[0], original.getX()[0]);
  int halo_after_flush = 0;
  container.forEachHaloParticle([&](Particle *) { halo_after_flush++; });
  EXPECT_EQ(halo_after_flush, 0);
}

TEST(LinkedCellContainerTest, OnlyReflectingFacesCreateGhosts) {
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});
  std::array<BoundaryCondition, 6> bc{};
  bc.fill(BoundaryCondition::Outflow);
  bc[static_cast<std::size_t>(Face::XMin)] = BoundaryCondition::Reflecting;
  container.setBoundaryConditions(bc);

  container.emplaceParticle({0.2, 1.5, 1.0}, {0.5, 0.0, 0.0}, 1.0);   // XMin reflecting
  container.emplaceParticle({2.8, 0.2, 1.0}, {-0.5, 0.0, 0.0}, 1.0);  // YMin outflow

  container.rebuild();

  int ghosts_on_negative_x = 0;
  int particles_with_negative_y = 0;
  container.forEachHaloParticle([&](Particle *p) {
    ghosts_on_negative_x += p->getX()[0] < 0.0 ? 1 : 0;
    particles_with_negative_y += p->getX()[1] < 0.0 ? 1 : 0;
  });

  EXPECT_EQ(container.size(), 2u);  // only the two originals are owned
  EXPECT_EQ(ghosts_on_negative_x, 1);
  EXPECT_EQ(particles_with_negative_y, 0);
}
TEST(LinkedCellContainerTest, CornerParticleMirroredCorrectly) {
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});
  std::array<BoundaryCondition, 6> bc{};
  bc.fill(BoundaryCondition::Outflow);
  bc[static_cast<std::size_t>(Face::XMin)] = BoundaryCondition::Reflecting;
  bc[static_cast<std::size_t>(Face::YMin)] = BoundaryCondition::Reflecting;
  container.setBoundaryConditions(bc);

  auto &corner = container.emplaceParticle({0.2, 0.2, 1.0}, {1.0, 2.0, 3.0}, 1.0);

  container.rebuild();

  EXPECT_EQ(container.size(), 1u);  // ghosts live in halo

  int ghosts_on_negative_x = 0;
  int ghosts_on_negative_y = 0;
  bool saw_x_ghost = false;
  bool saw_y_ghost = false;
  container.forEachHaloParticle([&](Particle *p) {
    ghosts_on_negative_x += p->getX()[0] < 0.0 ? 1 : 0;
    ghosts_on_negative_y += p->getX()[1] < 0.0 ? 1 : 0;
    if (p->getX()[0] < 0.0) {
      EXPECT_DOUBLE_EQ(p->getX()[0], -corner.getX()[0]);
      EXPECT_DOUBLE_EQ(p->getX()[1], corner.getX()[1]);
      EXPECT_DOUBLE_EQ(p->getV()[0], -corner.getV()[0]);
      EXPECT_DOUBLE_EQ(p->getV()[1], corner.getV()[1]);
      saw_x_ghost = true;
    }
    if (p->getX()[1] < 0.0) {
      EXPECT_DOUBLE_EQ(p->getX()[0], corner.getX()[0]);
      EXPECT_DOUBLE_EQ(p->getX()[1], -corner.getX()[1]);
      EXPECT_DOUBLE_EQ(p->getV()[0], corner.getV()[0]);
      EXPECT_DOUBLE_EQ(p->getV()[1], -corner.getV()[1]);
      saw_y_ghost = true;
    }
  });

  EXPECT_EQ(ghosts_on_negative_x, 1);
  EXPECT_EQ(ghosts_on_negative_y, 1);
  EXPECT_TRUE(saw_x_ghost);
  EXPECT_TRUE(saw_y_ghost);
}
TEST(LinkedCellContainerTest, NoneBoundaryDoesNotReflectHaloParticles) {
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});
  std::array<BoundaryCondition, 6> bc{};
  bc.fill(BoundaryCondition::Outflow);
  bc[static_cast<std::size_t>(Face::XMin)] = BoundaryCondition::None;
  container.setBoundaryConditions(bc);

  container.emplaceParticle({-0.05, 1.5, 1.5}, {0.0, 0.0, 0.0}, 1.0);  // halo on -x
  container.emplaceParticle({1.5, 1.5, 1.5}, {0.0, 0.0, 0.0}, 1.0);    // keep inside

  container.rebuild();

  EXPECT_EQ(container.size(), 1u);
  EXPECT_GT(container.begin()->getX()[0], 0.0);
}

TEST(LinkedCellContainerTest, ReflectingBoundaryMirrorsPositionAndVelocity) {
  LinkedCellContainer container(1.0, {3.0, 3.0, 3.0});
  std::array<BoundaryCondition, 6> bc{};
  bc.fill(BoundaryCondition::Outflow);
  bc[static_cast<std::size_t>(Face::XMin)] = BoundaryCondition::Reflecting;
  container.setBoundaryConditions(bc);

  auto &original = container.emplaceParticle({0.25, 1.2, 0.8}, {1.5, -0.4, 0.3}, 1.0);

  container.rebuild();

  EXPECT_EQ(container.size(), 1u);  // ghost stays in halo

  const auto &pos = container.begin()->getX();
  const auto &vel = container.begin()->getV();
  EXPECT_DOUBLE_EQ(pos[0], 0.25);
  EXPECT_DOUBLE_EQ(vel[0], 1.5);

  int ghosts_on_negative_x = 0;
  container.forEachHaloParticle([&](Particle *p) {
    const auto &ghost_pos = p->getX();
    const auto &ghost_vel = p->getV();
    ghosts_on_negative_x += ghost_pos[0] < 0.0 ? 1 : 0;
    EXPECT_DOUBLE_EQ(ghost_pos[0], -original.getX()[0]);
    EXPECT_DOUBLE_EQ(ghost_pos[1], original.getX()[1]);
    EXPECT_DOUBLE_EQ(ghost_pos[2], original.getX()[2]);
    EXPECT_DOUBLE_EQ(ghost_vel[0], -original.getV()[0]);
    EXPECT_DOUBLE_EQ(ghost_vel[1], original.getV()[1]);
    EXPECT_DOUBLE_EQ(ghost_vel[2], original.getV()[2]);
  });
  EXPECT_EQ(ghosts_on_negative_x, 1);
}