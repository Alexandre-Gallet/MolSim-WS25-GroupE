/**
 * @file CheckpointWriter.h
 * @brief Writer that dumps full particle phase space for restart.
 */
#pragma once

#include <string>

#include "OutputWriter.h"

namespace outputWriter {

class CheckpointWriter : public OutputWriter {
 public:
  CheckpointWriter() = default;
  ~CheckpointWriter() override = default;

  /**
   * @brief Write the complete particle state to a text checkpoint.
   *
   * Format:
   *   # Phase space checkpoint
   *   STATE <num_particles>
   *   x0 x1 x2 v0 v1 v2 f0 f1 f2 oldf0 oldf1 oldf2 mass type
   */
  void plotParticles(Container &particles, const std::string &filename, int iteration) override;
};

}  // namespace outputWriter
