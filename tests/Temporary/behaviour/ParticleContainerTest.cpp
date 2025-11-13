



#include <gtest/gtest.h>
#include "ParticleGenerator.h"
#include "ParticleContainer.h"


/*

   Behaviour test for the ParticleGenerator component in the MolSim project.

  This test verifies that the ParticleGenerator correctly generates a cuboid of particles
  with the expected number of particles and assigns non-zero velocities using the
  Maxwell-Boltzmann distribution.

 */

/*
  Ensures that ParticleGenerator creates the correct number of particles and applies
         a non-zero velocity distribution.

    **Test Idea:**
  Given a cuboid configuration (origin, dimensions, spacing, etc.), when the generator
  creates the particles, then:
   - The number of generated particles should equal the product of the given dimensions.
   - The velocities of at least some particles should not be all zero (indicating the
    application of Maxwell-Boltzmann thermal motion).
 */
TEST(ParticleGeneratorBehaviourTest, GeneratesCorrectNumberAndNonZeroVelocity) {
    /// Container to hold the generated particles.
    ParticleContainer container;

    /// Create generator instance.
    ParticleGenerator generator;

    /// Define cuboid parameters.
    const std::array<double, 3> origin{0.0, 0.0, 0.0};  ///< Lower-left corner
    const std::array<size_t, 3> dims{3, 2, 1};             ///< Number of particles per dimension
    const double h = 1.0;                               ///< Spacing between particles
    const double mass = 1.0;                            ///< Mass of each particle
    const std::array<double, 3> initVel{0.0, 0.0, 0.0}; ///< Initial base velocity
    const double mbMeanVelocity = 0.1;                  ///< Mean velocity for Brownian motion

    /// Generate the cuboid of particles with Maxwell-Boltzmann noise.
    generator.generateCuboid(container, origin, dims, h, mass, initVel, mbMeanVelocity);

    /// Verify that the total number of particles equals 3×2×1 = 6.
    EXPECT_EQ(container.size(), 6) << "ParticleGenerator did not produce the expected number of particles.";

    /// Check that at least one particle received a non-zero velocity.
    bool foundNonZeroVel = false;
    for (auto &p : container) {
        auto v = p.getV();
        if (v[0] != 0.0 || v[1] != 0.0 || v[2] != 0.0) {
            foundNonZeroVel = true;
            break;
        }
    }

    EXPECT_TRUE(foundNonZeroVel)
        << "All particle velocities are zero — Maxwell-Boltzmann distribution may not have been applied.";
}



TEST(ParticleGeneratorBehaviourTest, GeneratesCorrectGridPositions) {
  ParticleContainer c;
  ParticleGenerator g;
  g.generateCuboid(c, {0,0,0}, {3,3,1}, 1.0, 1.0, {0,0,0}, 0.1);

  for (auto &p : c) {
    auto x = p.getX();
    EXPECT_TRUE(fmod(x[0], 1.0) == 0.0);
    EXPECT_TRUE(fmod(x[1], 1.0) == 0.0);
  }
}


//Check that cuboid generator does not change initial velocities when brownian motion mean velocitiy is 0
TEST(ParticleGeneratorBehaviourTest, NoBrownianKeepsVelocityConstant) {
  ParticleContainer container;

  const std::array<double, 3> origin{0.0, 0.0, 0.0};
  const std::array<size_t, 3> N{3, 2, 1};
  const double h = 1.0;
  const double m = 1.0;
  const std::array<double, 3> v0{1.0, -0.5, 0.2};
  const double brownianMeanVelocity = 0.0;

  generateCuboid(container, origin, N, h, m, v0, brownianMeanVelocity);

  for (const auto &p : container) {
    EXPECT_DOUBLE_EQ(p.getV()[0], v0[0]);
    EXPECT_DOUBLE_EQ(p.getV()[1], v0[1]);
    EXPECT_DOUBLE_EQ(p.getV()[2], v0[2]);
  }
}




/*
 *LJ force should be approximately zero at the equilibrium distance r = 2^(1/6) * sigma.
 */
TEST(Behaviour_LennardJones, ForceNearEquilibriumIsSmall) {
  const double epsilon = 5.0;
  const double sigma = 1.0;
  const double m = 1.0;

  ParticleContainer container;
  Particle p1, p2;

  const double r_eq = std::pow(2.0, 1.0 / 6.0) * sigma;

  p1.setX({0.0, 0.0, 0.0});
  p2.setX({r_eq, 0.0, 0.0});
  p1.setV({0.0, 0.0, 0.0});
  p2.setV({0.0, 0.0, 0.0});
  p1.setM(m);
  p2.setM(m);

  container.addParticle(p1);
  container.addParticle(p2);

  for (auto &p : container) {
    p.setF({0.0, 0.0, 0.0});
  }

  computeForcesLennardJones(container, epsilon, sigma);
  const auto &p1_after = container[0];
  const auto &f = p1_after.getF();
  double f_norm = std::sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);

  EXPECT_NEAR(f_norm, 0.0, 1e-4);
}

TEST(SimulationTest, HalleysCometSimulationRunsCorrectly) {

  //  Input data (char* -> stringstream)
  const char *inputData =
      "4\n"
      "0.0 0.0 0.0      0.0 0.0 0.0     1.0\n"
      "0.0 1.0 0.0     -1.0 0.0 0.0     3.0e-6\n"
      "0.0 5.36 0.0    -0.425 0.0 0.0   9.55e-4\n"
      "34.75 0.0 0.0    0.0 0.0296 0.0  1.0e-14\n";

  std::stringstream ss(inputData);

  //  Parse into container
  ParticleContainer container;
  FileReader reader;
  reader.readFile(ss, container);

  ASSERT_EQ(container.size(), 4) << "Should read 4 celestial bodies.";

  // . Simulation parameters
  const double dt = 0.014;
  const double t_end = 1000.0;

  Simulation sim(dt, t_end);
  sim.setContainer(&container);

  //   Run full simulation
  ASSERT_NO_THROW(sim.runSimulation())
      << "Simulation should run without crashes.";

  //  Verify results
  for (auto &p : container) {
    EXPECT_FALSE(std::isnan(p.x()[0]));
    EXPECT_FALSE(std::isnan(p.v()[0]));
  }

  //  Special checks for physical plausibility -
  auto &sun = container[0];
  EXPECT_NEAR(sun.x()[0], 0.0, 2.0);

  // Earth-like particle should stay within a reasoned orbit radius
  auto &earth = container[1];
  EXPECT_GT(earth.x()[0]*earth.x()[0] + earth.x()[1]*earth.x()[1], 0.1);

  // Jupiter-like orbit should remain bounded.
  auto &jupiter = container[3];
  EXPECT_LT(jupiter.x()[0], 200.0);

  // Halley's comet orbit should remain bounded as well.
  auto &halley = container[2];
  EXPECT_LT(std::abs(halley.x()[0]), 200.0);
}








