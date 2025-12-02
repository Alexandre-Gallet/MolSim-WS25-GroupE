

#include <gtest/gtest.h>

#include <cmath>
#include <sstream>

#include "../../src/Generator/ParticleGenerator.h"
#include "Generator/CuboidGenerator.h"
#include "ParticleContainer.h"
#include "Simulation/Simulation.h"
#include "inputReader/FileReader.h"


/*
#include "GParticleGenerator.h"
#include "Simulation/Simulation.h"
#include "inputReader/FileReader.h"


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

    /// Define cuboid parameters.
    const std::array<double, 3> origin{0.0, 0.0, 0.0};  ///< Lower-left corner
    const std::array<size_t, 3> dims{3, 2, 1};             ///< Number of particles per dimension
    const double h = 1.0;                               ///< Spacing between particles
    const double mass = 1.0;                            ///< Mass of each particle
    const std::array<double, 3> initVel{0.0, 0.0, 0.0}; ///< Initial base velocity
    const double mbMeanVelocity = 0.1;                  ///< Mean velocity for Brownian motion

    /// Generate the cuboid of particles with Maxwell-Boltzmann noise.
    CuboidGenerator::generateCuboid(container, origin, dims, h, mass, initVel, mbMeanVelocity);

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
  CuboidGenerator::generateCuboid(c, {0,0,0}, {3,3,1}, 1.0, 1.0, {0,0,0}, 0.1);

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

  CuboidGenerator::generateCuboid(container, origin, N, h, m, v0, brownianMeanVelocity);

  for (const auto &p : container) {
    EXPECT_DOUBLE_EQ(p.getV()[0], v0[0]);
    EXPECT_DOUBLE_EQ(p.getV()[1], v0[1]);
    EXPECT_DOUBLE_EQ(p.getV()[2], v0[2]);
  }
}
















