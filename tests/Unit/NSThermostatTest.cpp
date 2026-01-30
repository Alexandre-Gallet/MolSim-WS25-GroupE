
#include <gtest/gtest.h>

#include "Container/ParticleContainer.h"
#include "utils/ArrayUtils.h"
#include "utils/NanoScaleThermostat.h"

class NanoScaleThermostatTest : public ::testing::Test {};

// Ensures the thermostat ignores wall particles (Type 1) and does not scale their velocity.
TEST_F(NanoScaleThermostatTest, TestIgnoreWalls) {
  ParticleContainer container;
  // Fluid particle
  container.addParticle(Particle({5, 5, 5}, {0.0, 10.0, 0.0}, 1.0, 0));
  // Wall particle
  container.addParticle(Particle({0, 0, 0}, {0.0, 0.0, 0.0}, 1.0, 1));
  // High target temperature to force scaling
  NanoScaleThermostat thermo(10.0, 3, 1, 100.0, 100.0, false);
  thermo.apply(container, 0);

  for (const auto &p : container) {
    if (p.getType() == 1) {
      // Wall velocity must remain exactly zero
      EXPECT_DOUBLE_EQ(p.getV()[0], 0.0);
      EXPECT_DOUBLE_EQ(p.getV()[1], 0.0);
      EXPECT_DOUBLE_EQ(p.getV()[2], 0.0);
    }
  }
}

// Verifies that the thermostat scales relative to the average velocity.
// If all fluid particles move at the same speed, thermal motion is 0, and temperature should be 0.
TEST_F(NanoScaleThermostatTest, TestFlowVelocity) {
  ParticleContainer container;
  // Two fluid particles moving at the same speed
  // Their thermal motion is zero.
  container.addParticle(Particle({5, 5, 5}, {0.0, 10.0, 0.0}, 1.0, 0));
  container.addParticle(Particle({6, 6, 6}, {0.0, 10.0, 0.0}, 1.0, 0));

  NanoScaleThermostat thermo(0.0, 3, 1, 10.0, 1.0, false);
  thermo.apply(container, 0);

  for (const auto &p : container) {
    if (p.getType() == 0) {
      // Since thermal motion is 0, scaling should equal the average velocity
      EXPECT_NEAR(p.getV()[1], 10.0, 1e-9);
    }
  }
}

// Verifies the Beta scaling calculation specifically for thermal motion.
TEST_F(NanoScaleThermostatTest, TestThermalScaling) {
  ParticleContainer container;

  container.addParticle(Particle({1, 1, 1}, {0.0, 6.0, 0.0}, 1.0, 0));
  container.addParticle(Particle({2, 2, 2}, {0.0, 4.0, 0.0}, 1.0, 0));

  NanoScaleThermostat thermo(0.333333, 3, 1, 1.333333, 10.0, false);
  thermo.apply(container, 0);

  auto it = container.begin();
  // New velocity
  EXPECT_NEAR(it->getV()[1], 7.0, 1e-4);
}