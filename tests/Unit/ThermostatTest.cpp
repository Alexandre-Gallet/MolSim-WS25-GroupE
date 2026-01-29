
#include <gtest/gtest.h>

#include <cmath>

#include "Container/Container.h"
#include "Container/Particle.h"
#include "Container/ParticleContainer.h"
#include "utils/Thermostat.h"

class ThermostatTest : public ::testing::Test {
 protected:
  void setupContainer(Container &container, double velocity) {
    Particle p1({0, 0, 0}, {velocity, 0, 0}, 1.0);
    Particle p2({1, 1, 1}, {0, velocity, 0}, 1.0);
    container.addParticle(p1);
    container.addParticle(p2);
  }
};

TEST_F(ThermostatTest, TestHoldsTemperature) {
  ParticleContainer container;
  setupContainer(container, 1.0);
  double t_target = 1.0 / 3.0;
  Thermostat thermostat(t_target, 3, 1, t_target, 0.1, false);
  thermostat.apply(container, 1);

  // Velocity shouldn't change
  auto iter = container.begin();
  EXPECT_NEAR(iter->getV()[0], 1.0, 1e-9);
}

TEST_F(ThermostatTest, TestCoolingTemperature) {
  ParticleContainer container;
  setupContainer(container, 2.0);

  double t_target = 0.5;
  Thermostat thermo(1.333, 3, 1, t_target, 100.0, false);
  thermo.apply(container, 1);

  // Check if new temperature is t_target
  auto iter = container.begin();
  double expected_beta = std::sqrt(0.5 / 1.3333333333);
  EXPECT_NEAR(iter->getV()[0], 2.0 * expected_beta, 1e-4);
}


TEST_F(ThermostatTest, TestHeatingTemperature) {
  ParticleContainer container;
  container.addParticle(Particle({0, 0, 0}, {1.0, 0.0, 0.0}, 1.0, 0));
  container.addParticle(Particle({1, 1, 1}, {0.0, 1.0, 0.0}, 1.0, 1));

  double t_init = 1.0 / 3.0;
  double t_target = 1.0;
  double delta_t = 0.1;  // Test gradual heating

  Thermostat thermostat(t_init, 3, 1, t_target, delta_t, false);
  thermostat.apply(container, 1);

  double expected_beta = std::sqrt(0.4333333333 / 0.3333333333);
  double expected_v = 1.0 * expected_beta;

  auto iter = container.begin();
  EXPECT_NEAR(iter->getV()[0], expected_v, 1e-4);
}