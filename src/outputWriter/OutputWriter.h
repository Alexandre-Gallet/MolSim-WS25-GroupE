
#pragma once

#include "Particle.h"

namespace outputWriter {
/**
 * This abstract class defines a common interface for all output writers
 */
class OutputWriter {
 public:
  OutputWriter() = default;

  virtual ~OutputWriter() = default;
  /**
   * @brief Write the given particles to an output file.
   * @param particles articles to write to output
   * @param filename Output filename
   * @param iteration Current iteration number
   */
  virtual void plotParticles(const std::list<Particle> &particles, const std::string &filename, int iteration) = 0;
};
}  // namespace outputWriter
