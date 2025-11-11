/**
 * @file test_particle_generator.cpp
 * @brief Behaviour test for the ParticleGenerator component in the MolSim project.
 *
 * This test verifies that the ParticleGenerator correctly generates a cuboid of particles
 * with the expected number of particles and assigns non-zero velocities using the
 * Maxwell-Boltzmann distribution.
 *
 * Unlike a unit test, this behaviour test checks the *observable behaviour* of the
 * ParticleGenerator as a whole, rather than the correctness of a single internal function.
 */

#include <gtest/gtest.h>
#include "ParticleGenerator.h"
#include "ParticleContainer.h"

/**
 * @test ParticleGeneratorBehaviourTest.GeneratesCorrectNumberAndNonZeroVelocity
 * @brief Ensures that ParticleGenerator creates the correct number of particles and applies
 *        a non-zero velocity distribution.
 *
 * **Test Idea:**
 * Given a cuboid configuration (origin, dimensions, spacing, etc.), when the generator
 * creates the particles, then:
 * - The number of generated particles should equal the product of the given dimensions.
 * - The velocities of at least some particles should not be all zero (indicating the
 *   application of Maxwell-Boltzmann thermal motion).
 *
 * **Type:** Behaviour Test
 * **Rationale:** Tests the overall behaviour of `ParticleGenerator` rather than the
 * correctness of internal formulas. It observes the *expected effects* (number of particles
 * and presence of random velocity) from a user's perspective.
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











