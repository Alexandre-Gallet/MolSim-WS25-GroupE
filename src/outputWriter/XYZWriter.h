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
#include "Particle.h"

namespace outputWriter {

class XYZWriter : public OutputWriter {
 public:
  XYZWriter();

  ~XYZWriter() override;

  void plotParticles(const std::list<Particle> &particles, const std::string &filename, int iteration) override;
};

}  // namespace outputWriter
