#include <gtest/gtest.h>

#include <array>
#include <vector>

#include "Container/LinkedCellContainer.h"
#include "ForceCalculation/LennardJones.h"
#include "inputReader/SimulationConfig.h"
#include "utils/OpenMPCompat.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <cmath>

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

}  // namespace

TEST(ParallelForcesTest, PairStaticMatchesSerialForcesLinkedCell) {
  // Only meaningful if OpenMP is available; otherwise PairStatic takes serial fallback.
  if (!OpenMPCompat::enabled()) {
    GTEST_SKIP() << "OpenMP not enabled in this build; skipping true parallel-path test.";
  }

#ifdef _OPENMP
  // Force at least 2 threads so we actually exercise the parallel path
  omp_set_dynamic(0);
  omp_set_num_threads(2);
#endif

  if (OpenMPCompat::maxThreads() < 2) {
    GTEST_SKIP() << "OpenMP enabled but maxThreads < 2; skipping true parallel-path test.";
  }


  // Small domain and cutoff -> a few cells, easy neighbor traversal
  LinkedCellContainer lc(/*r_cutoff=*/3.0, /*domain_size=*/{10.0, 10.0, 10.0});

  // Boundary conditions irrelevant here, but keep stable
  lc.setBoundaryConditions({BoundaryCondition::None, BoundaryCondition::None,
                            BoundaryCondition::None, BoundaryCondition::None,
                            BoundaryCondition::None, BoundaryCondition::None});

  // Place a few particles well inside domain
  lc.emplaceParticle({2.0, 2.0, 2.0}, {0.0, 0.0, 0.0}, 1.0, /*type=*/0);
  lc.emplaceParticle({3.0, 2.2, 2.1}, {0.0, 0.0, 0.0}, 1.0, /*type=*/2);
  lc.emplaceParticle({7.0, 7.0, 7.0}, {0.0, 0.0, 0.0}, 2.0, /*type=*/0);
  lc.emplaceParticle({6.5, 7.2, 6.8}, {0.0, 0.0, 0.0}, 2.0, /*type=*/2);
  // Add a "wall" particle (type==1). It should remain force-free.
  lc.emplaceParticle({5.0, 5.0, 5.0}, {0.0, 0.0, 0.0}, 1.0, /*type=*/1);


  // Ensure cells are populated consistently
  lc.rebuild();

  std::size_t wall_idx = static_cast<std::size_t>(-1);
  for (auto &p : lc) {
    if (p.getType() == 1) wall_idx = static_cast<std::size_t>(p.getOwnedIndex());
  }
  ASSERT_NE(wall_idx, static_cast<std::size_t>(-1)) << "Wall particle not found";


  LennardJones lj;
  lj.setGravity({0.0, 0.0, 0.0});

  // Optional: type overrides matching your YAML patterns
  lj.setTypeParameters({LJTypeParams{0, 1.0, 1.2}, LJTypeParams{2, 1.0, 1.1}});

  // --- Serial baseline ---
  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::None);
  lj.calculateF(lc);
  const auto f_serial = snapshotForces(lc);

  // --- Parallel method ---
  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::PairStatic);
  lj.calculateF(lc);
  const auto f_parallel = snapshotForces(lc);

  ASSERT_EQ(f_serial.size(), f_parallel.size());

  // Wall particle should have exactly zero force in both modes
  ASSERT_LT(wall_idx, f_serial.size());
  EXPECT_DOUBLE_EQ(f_serial[wall_idx][0], 0.0);
  EXPECT_DOUBLE_EQ(f_serial[wall_idx][1], 0.0);
  EXPECT_DOUBLE_EQ(f_serial[wall_idx][2], 0.0);

  ASSERT_LT(wall_idx, f_parallel.size());
  EXPECT_DOUBLE_EQ(f_parallel[wall_idx][0], 0.0);
  EXPECT_DOUBLE_EQ(f_parallel[wall_idx][1], 0.0);
  EXPECT_DOUBLE_EQ(f_parallel[wall_idx][2], 0.0);

  // Tolerance: parallel accumulation changes summation order slightly.
  // Keep this tight but not brittle.
  const double absTol = 1e-10;
  const double relTol = 1e-9;

  for (std::size_t i = 0; i < f_serial.size(); ++i) {
    if (i == wall_idx) continue;
    for (int k = 0; k < 3; ++k) {
      const double a = f_serial[i][k];
      const double b = f_parallel[i][k];
      const double tol = absTol + relTol * std::max(std::abs(a), std::abs(b));
      EXPECT_NEAR(a, b, tol) << "Mismatch at particle " << i << ", component " << k;
    }
  }
}
