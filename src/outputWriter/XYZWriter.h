/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>
#include <list>

#include "../Container/Particle.h"
#include "../Container/ParticleContainer.h"
#include "OutputWriter.h"

namespace outputWriter {

class XYZWriter : public OutputWriter {
 public:
  XYZWriter();

  ~XYZWriter() override;

  void plotParticles(Container &particles, const std::string &filename, int iteration) override;
};

}  // namespace outputWriter
