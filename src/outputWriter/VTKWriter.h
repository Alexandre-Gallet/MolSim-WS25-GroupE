/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once
#ifdef ENABLE_VTK_OUTPUT

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <list>
#include <string>

#include "Container/Particle.h"
#include "Container/ParticleContainer.h"
#include "OutputWriter.h"

namespace outputWriter {

/**
 * This class implements the functionality to generate vtk output from
 * particles using the official VTK library.
 */
class VTKWriter : public OutputWriter {
 public:
  VTKWriter() = default;
  ~VTKWriter() override = default;

  // Delete copy constructor and assignment operator
  VTKWriter(const VTKWriter &) = delete;
  VTKWriter &operator=(const VTKWriter &) = delete;

  /**
   * Write VTK output of particles.
   * @param particles Particles to add to the output
   * @param filename Output filename
   * @param iteration Current iteration number
   */
  void plotParticles(Container &particles, const std::string &filename, int iteration) override;
};

}  // namespace outputWriter
#endif