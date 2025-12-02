/*
 * XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>
#include <list>

#include "OutputWriter.h"
#include "../Container/Particle.h"
#include "../Container/ParticleContainer.h"

namespace outputWriter {

class XYZWriter : public OutputWriter {
 public:
  XYZWriter();

  ~XYZWriter() override;

  void plotParticles(ParticleContainer &particles, const std::string &filename, int iteration) override;
};

}  // namespace outputWriter
