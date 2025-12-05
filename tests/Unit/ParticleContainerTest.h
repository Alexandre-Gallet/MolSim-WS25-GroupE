#include <array>
#include "gtest/gtest.h"
#include "../../src/Container/ParticleContainer.h"
#include "../../src/Container/Particle.h"

/*
Test fixture providing a ParticleContainer with two particles.

   This fixture is used for tests that need a pre-filled container.
   It creates two particles at positions (0,0,0) and (1,0,0).
 */
class ParticleContainerFixture : public ::testing::Test {
protected:
    void SetUp() override {
        std::array<double, 3> x1{0, 0, 0}, x2{1, 0, 0}, v{0, 0, 0};
        pc.addParticle(Particle{x1, v, 1.0, 0});
        pc.addParticle(Particle{x2, v, 1.0, 0});
    }

    //The container under test.

    ParticleContainer pc;
};