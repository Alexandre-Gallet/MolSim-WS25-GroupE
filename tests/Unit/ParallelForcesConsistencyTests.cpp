#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "Container/LinkedCellContainer.h"
#include "ForceCalculation/LennardJones.h"
#include "inputReader/SimulationConfig.h"
#include "utils/OpenMPCompat.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

// Helper: reset forces like calculateF() expects
void resetForces(Container &c) {
  for (auto &p : c) {
    p.setOldF(p.getF());
    p.setF({0.0, 0.0, 0.0});
  }
}

// Helper: snapshot forces by owned index (safe for LinkedCellContainer owned particles)
std::vector<std::array<double, 3>> snapshotForces(Container &c) {
  std::vector<std::array<double, 3>> out(c.size());
  for (auto &p : c) {
    const std::size_t idx = static_cast<std::size_t>(p.getOwnedIndex());
    if (idx < out.size()) out[idx] = p.getF();
  }
  return out;
}

void expectForceArraysNear(const std::vector<std::array<double, 3>> &a, const std::vector<std::array<double, 3>> &b,
                           double absTol, double relTol) {
  ASSERT_EQ(a.size(), b.size());
  for (std::size_t i = 0; i < a.size(); ++i) {
    for (int k = 0; k < 3; ++k) {
      const double x = a[i][k];
      const double y = b[i][k];
      const double tol = absTol + relTol * std::max(std::abs(x), std::abs(y));
      EXPECT_NEAR(x, y, tol) << "Mismatch at particle " << i << ", component " << k;
    }
  }
}

// If you want to ensure we truly run >1 thread for OpenMP path:
void requireAtLeastTwoThreadsOrSkip() {
  if (!OpenMPCompat::enabled()) {
    GTEST_SKIP() << "OpenMP not enabled in this build; skipping true parallel-path test.";
  }
#ifdef _OPENMP
  omp_set_dynamic(0);
  omp_set_num_threads(2);
#endif
  if (OpenMPCompat::maxThreads() < 2) {
    GTEST_SKIP() << "OpenMP enabled but maxThreads < 2; skipping true parallel-path test.";
  }
}

}  // namespace

// ------------------------------------------------------------
// TEST 1: PairStatic vs Serial, periodic across boundary
// ------------------------------------------------------------
TEST(ParallelForcesTest, PairStaticMatchesSerial_WithPeriodicGhostsAcrossBoundary) {
  requireAtLeastTwoThreadsOrSkip();

  // Domain 10x10x10, cutoff 3 -> periodic ghosts created within layer=r_cutoff.
  LinkedCellContainer lc(/*r_cutoff=*/3.0, /*domain_size=*/{10.0, 10.0, 10.0});

  // Periodic in X and Y, none in Z (but Z periodic would also work)
  lc.setBoundaryConditions({
      BoundaryCondition::Periodic, BoundaryCondition::Periodic,  // X min/max
      BoundaryCondition::Periodic, BoundaryCondition::Periodic,  // Y min/max
      BoundaryCondition::None, BoundaryCondition::None           // Z min/max
  });

  // Two particles close across X boundary:
  // pA at x=0.2, pB at x=9.9 => wrapped distance 0.3 (within cutoff)
  lc.emplaceParticle({0.2, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, /*type=*/0);
  lc.emplaceParticle({9.9, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, /*type=*/2);

  // A couple extra interior particles so it isn't too degenerate
  lc.emplaceParticle({5.0, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, /*type=*/0);
  lc.emplaceParticle({6.2, 5.3, 5.1}, {0.0, 0.0, 0.0}, 1.0, /*type=*/2);

  lc.rebuild();

  LennardJones lj;
  lj.setGravity({0.0, 0.0, 0.0});
  lj.setTypeParameters({LJTypeParams{0, 1.0, 1.2}, LJTypeParams{2, 1.0, 1.1}});

  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::None);
  lj.calculateF(lc);
  const auto f_serial = snapshotForces(lc);

  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::PairStatic);
  lj.calculateF(lc);
  const auto f_parallel = snapshotForces(lc);

  // summation order differs slightly
  expectForceArraysNear(f_serial, f_parallel, /*absTol=*/1e-10, /*relTol=*/1e-9);
}

// ------------------------------------------------------------
// TEST 2: CellDynamic vs Serial, no boundaries (basic correctness)
// ------------------------------------------------------------
TEST(ParallelForcesTest, CellDynamicMatchesSerial_NoBoundaries) {
  requireAtLeastTwoThreadsOrSkip();

  LinkedCellContainer lc(/*r_cutoff=*/3.0, /*domain_size=*/{10.0, 10.0, 10.0});
  lc.setBoundaryConditions({BoundaryCondition::None, BoundaryCondition::None, BoundaryCondition::None,
                            BoundaryCondition::None, BoundaryCondition::None, BoundaryCondition::None});

  lc.emplaceParticle({2.0, 2.0, 2.0}, {0, 0, 0}, 1.0, 0);
  lc.emplaceParticle({3.0, 2.2, 2.1}, {0, 0, 0}, 1.0, 2);
  lc.emplaceParticle({7.0, 7.0, 7.0}, {0, 0, 0}, 2.0, 0);
  lc.emplaceParticle({6.5, 7.2, 6.8}, {0, 0, 0}, 2.0, 2);

  // Optional wall particle if you want:
  // lc.emplaceParticle({5.0,5.0,5.0}, {0,0,0}, 999999.0, 1);

  lc.rebuild();

  LennardJones lj;
  lj.setGravity({0.0, 0.0, 0.0});
  lj.setTypeParameters({LJTypeParams{0, 1.0, 1.2}, LJTypeParams{2, 1.0, 1.1}});

  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::None);
  lj.calculateF(lc);
  const auto f_serial = snapshotForces(lc);

  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::CellDynamic);
  lj.calculateF(lc);
  const auto f_parallel = snapshotForces(lc);

  expectForceArraysNear(f_serial, f_parallel, /*absTol=*/1e-10, /*relTol=*/1e-9);
}

// ------------------------------------------------------------
// TEST 3: CellDynamic vs Serial, periodic across boundary (ghost correctness)
// ------------------------------------------------------------
TEST(ParallelForcesTest, CellDynamicMatchesSerial_WithPeriodicGhostsAcrossBoundary) {
  requireAtLeastTwoThreadsOrSkip();

  LinkedCellContainer lc(/*r_cutoff=*/3.0, /*domain_size=*/{10.0, 10.0, 10.0});
  lc.setBoundaryConditions({
      BoundaryCondition::Periodic, BoundaryCondition::Periodic,  // X
      BoundaryCondition::Periodic, BoundaryCondition::Periodic,  // Y
      BoundaryCondition::None, BoundaryCondition::None           // Z
  });

  lc.emplaceParticle({0.2, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, 0);
  lc.emplaceParticle({9.9, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, 2);
  lc.emplaceParticle({5.0, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, 0);
  lc.emplaceParticle({6.2, 5.3, 5.1}, {0.0, 0.0, 0.0}, 1.0, 2);

  lc.rebuild();

  LennardJones lj;
  lj.setGravity({0.0, 0.0, 0.0});
  lj.setTypeParameters({LJTypeParams{0, 1.0, 1.2}, LJTypeParams{2, 1.0, 1.1}});

  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::None);
  lj.calculateF(lc);
  const auto f_serial = snapshotForces(lc);

  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::CellDynamic);
  lj.calculateF(lc);
  const auto f_parallel = snapshotForces(lc);

  expectForceArraysNear(f_serial, f_parallel, /*absTol=*/1e-10, /*relTol=*/1e-9);
}
