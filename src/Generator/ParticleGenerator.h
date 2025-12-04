// ParticleGenerator.h
#pragma once

#include "ParticleContainer.h"

/**
 * @brief Abstract base class for all particle generators.
 */
class ParticleGenerator {
 public:
  virtual ~ParticleGenerator() = default;

  /**
   * @brief Generate particles and insert them into the given container.
   */
  virtual void generate(ParticleContainer &container) const = 0;
};
