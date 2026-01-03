//
// Created by darig on 12/29/2025.
//

#include "Thermostat.h"

#include "ArrayUtils.h"
#include "MaxwellBoltzmannDistribution.h"

Thermostat::~Thermostat() = default;
 Thermostat::Thermostat(Container &particles, double T_init, size_t dimensions, size_t n_thermostat, double T_target, double delta_T,
                        bool brownian_motion)
     : particles(particles),
       T_init(T_init),
       delta_T(delta_T),
       T_target(T_target),
       dimensions(dimensions),
       n_thermostat(n_thermostat),
       brownian_motion(brownian_motion) {
   if (T_target < 0) {
     this->T_target = T_init;
   } else {
     this->T_target = T_target;
   }
 }
void Thermostat::initializeBrownianMotion() const {
  for (auto p : particles) {
    double factor = std::sqrt(T_init / p.getM());
    p.setV(maxwellBoltzmannDistributedVelocity(factor, dimensions));
  }
 }
double Thermostat::calculateKineticEnergy() {
   double kineticEnergy = 0.;
   for (auto p : particles) {
     kineticEnergy += ArrayUtils::L2Norm(p.getV()) * ArrayUtils::L2Norm(p.getV()) * p.getM() / 2;
   }
   return kineticEnergy;
 }
double Thermostat::calculateTemperature() {
   double kineticEnergy = calculateKineticEnergy();
   double temperature = 2 * kineticEnergy / (dimensions * particles.size() * 1);
   return temperature;
 }
void Thermostat::calculateScalingFactor() {
   double T_current = calculateTemperature();
   if (T_current == 0.) {
     return;
   }
   double diff = T_target - T_current;
   if (std::abs(diff) > delta_T) {
     if (diff > 0.) {
       diff = delta_T;
     } else {
       diff = -delta_T;
     }
   }
   double T_new = T_current + diff;
   double scalingFactor = std::sqrt(T_new / T_current);
   for (auto p : particles) {
     std::array<double, 3> vel = ArrayUtils::elementWiseScalarOp(scalingFactor, p.getV(), std::multiplies<>());
     p.setV(vel);
   }
 }
void Thermostat::apply(Container &particles, size_t step, size_t dimensions) {
   if (step % n_thermostat != 0) {
     return;
   }
   if (step == 0 && brownian_motion) {
     initializeBrownianMotion();
   }

 }






