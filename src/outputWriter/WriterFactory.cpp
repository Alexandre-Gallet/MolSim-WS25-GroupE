
#include "WriterFactory.h"

#include <memory>

#include "outputWriter/CheckpointWriter.h"
#include "outputWriter/OutputFormat.h"
#include "outputWriter/OutputWriter.h"
#ifdef ENABLE_VTK_OUTPUT
#include "VTKWriter.h"
#endif
#include "XYZWriter.h"

std::unique_ptr<outputWriter::OutputWriter> WriterFactory::createWriter(OutputFormat format) {
  switch (format) {
    case OutputFormat::XYZ:
      return std::make_unique<outputWriter::XYZWriter>();
    case OutputFormat::VTK:
// use macros to make vtk output optional and avoid build errors
#ifdef ENABLE_VTK_OUTPUT
      return std::make_unique<outputWriter::VTKWriter>();
#endif
    case OutputFormat::Checkpoint:
      return std::make_unique<outputWriter::CheckpointWriter>();
    default:
      return std::make_unique<outputWriter::XYZWriter>();
  }
}
