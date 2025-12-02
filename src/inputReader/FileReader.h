/*
 * FileReader.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <list>

#include "../Container/Container.h"
#include "../Container/Particle.h"
#include "../Container/ParticleContainer.h"

class FileReader {
 public:
  FileReader();
  virtual ~FileReader();

  void readFile(Container &particles, char *filename);
};
