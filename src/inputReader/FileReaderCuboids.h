/**
 * @file FileReaderCuboid.h
 */
#pragma once

#include "../ParticleContainer.h"
#include "Cuboid.h"

class FileReaderCuboid {
 public:
  FileReaderCuboid();
  virtual ~FileReaderCuboid();

  static void readFile(std::vector<Cuboid> &cuboids, char *filename);
};
