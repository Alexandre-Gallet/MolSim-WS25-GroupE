/**
 * @file CheckpointReader.h
 * @brief Reader for checkpoint files produced by CheckpointWriter.
 */
#pragma once

#include <string>

#include "../Container/ParticleContainer.h"

namespace inputReader {
class CheckpointReader {
 public:
  CheckpointReader() = default;
  ~CheckpointReader() = default;

  /**
   * @brief Load particle phase space from a checkpoint file.
   * @param particles Container to fill (cleared before loading).
   * @param filename Path to checkpoint file.
   */
  static void readFile(Container &particles, const std::string &filename);
};
}  // namespace inputReader
