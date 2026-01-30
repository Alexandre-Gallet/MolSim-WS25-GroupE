#include <gtest/gtest.h>

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

void resetForces(Container &c) {
  for (auto &p : c) {
    p.setOldF(p.getF());
    p.setF({0.0, 0.0, 0.0});
  }
}

std::vector<std::array<double, 3>> snapshotForces(Container &c) {
  std::vector<std::array<double, 3>> out(c.size());
  for (auto &p : c) {
    const std::size_t idx = static_cast<std::size_t>(p.getOwnedIndex());
    if (idx < out.size()) out[idx] = p.getF();
  }
  return out;
}

}  // namespace

TEST(ParallelForcesTest, CellDynamicMatchesSerialForcesLinkedCell) {
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

  LinkedCellContainer lc(/*r_cutoff=*/3.0, /*domain_size=*/{10.0, 10.0, 10.0});
  lc.setBoundaryConditions({BoundaryCondition::None, BoundaryCondition::None, BoundaryCondition::None,
                            BoundaryCondition::None, BoundaryCondition::None, BoundaryCondition::None});

  lc.emplaceParticle({2.0, 2.0, 2.0}, {0.0, 0.0, 0.0}, 1.0, 0);
  lc.emplaceParticle({3.0, 2.2, 2.1}, {0.0, 0.0, 0.0}, 1.0, 2);
  lc.emplaceParticle({7.0, 7.0, 7.0}, {0.0, 0.0, 0.0}, 2.0, 0);
  lc.emplaceParticle({6.5, 7.2, 6.8}, {0.0, 0.0, 0.0}, 2.0, 2);

  // Wall particle (type==1) must remain force-free.
  lc.emplaceParticle({5.0, 5.0, 5.0}, {0.0, 0.0, 0.0}, 999999.0, 1);

  lc.rebuild();

  std::size_t wall_idx = 0;
  for (auto &p : lc) {
    if (p.getType() == 1) wall_idx = static_cast<std::size_t>(p.getOwnedIndex());
  }

  LennardJones lj;
  lj.setGravity({0.0, 0.0, 0.0});
  lj.setTypeParameters({LJTypeParams{0, 1.0, 1.2}, LJTypeParams{2, 1.0, 1.1}});

  // Serial baseline
  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::None);
  lj.calculateF(lc);
  const auto f_serial = snapshotForces(lc);

  // CellDynamic
  resetForces(lc);
  lj.setParallelMethod(ParallelMethod::CellDynamic);
  lj.calculateF(lc);
  const auto f_dyn = snapshotForces(lc);

  ASSERT_EQ(f_serial.size(), f_dyn.size());

  ASSERT_LT(wall_idx, f_serial.size());
  EXPECT_DOUBLE_EQ(f_serial[wall_idx][0], 0.0);
  EXPECT_DOUBLE_EQ(f_serial[wall_idx][1], 0.0);
  EXPECT_DOUBLE_EQ(f_serial[wall_idx][2], 0.0);

  ASSERT_LT(wall_idx, f_dyn.size());
  EXPECT_DOUBLE_EQ(f_dyn[wall_idx][0], 0.0);
  EXPECT_DOUBLE_EQ(f_dyn[wall_idx][1], 0.0);
  EXPECT_DOUBLE_EQ(f_dyn[wall_idx][2], 0.0);

  const double absTol = 1e-10;
  const double relTol = 1e-9;

  for (std::size_t i = 0; i < f_serial.size(); ++i) {
    for (int k = 0; k < 3; ++k) {
      const double a = f_serial[i][k];
      const double b = f_dyn[i][k];
      const double tol = absTol + relTol * std::max(std::abs(a), std::abs(b));
      EXPECT_NEAR(a, b, tol) << "Mismatch at particle " << i << ", component " << k;
    }
  }
}
