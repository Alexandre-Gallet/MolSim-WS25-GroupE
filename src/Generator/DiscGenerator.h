// DiscGenerator.h
#pragma once

#include <array>
#include "ParticleGenerator.h"
#include "ParticleContainer.h"

/**
 * @brief Generates particles arranged in a 2D disc.
 *
 * The disc is generated in the x-y plane; z remains constant.
 */
class DiscGenerator : public ParticleGenerator {
public:
  DiscGenerator(const std::array<double, 3>& center,
                int radiusCells,
                double h,
                double mass,
                const std::array<double, 3>& baseVelocity,
                int type = 0)
      : center_(center),
        radiusCells_(radiusCells),
        h_(h),
        mass_(mass),
        baseVelocity_(baseVelocity),
        type_(type) {}

  void generate(ParticleContainer& container) const override;

private:
  std::array<double, 3> center_;
  int radiusCells_;
  double h_;
  double mass_;
  std::array<double, 3> baseVelocity_;
  int type_;
};
