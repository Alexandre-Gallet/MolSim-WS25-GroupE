/**
 * @file FileReaderCuboid.h
 */
#pragma once

#include "../Container/ParticleContainer.h"
#include "Cuboid.h"

class FileReaderCuboid {
 public:
  FileReaderCuboid();
  virtual ~FileReaderCuboid();

  static void readFile(std::vector<Cuboid> &cuboids, char *filename);
};
