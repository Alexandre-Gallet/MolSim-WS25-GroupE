#include "WriterFactory.h"

#include "VTKWriter.h"  // <-- always included
#include "outputWriter/CheckpointWriter.h"
#include "outputWriter/XYZWriter.h"

std::unique_ptr<outputWriter::OutputWriter> WriterFactory::createWriter(OutputFormat format) {
  switch (format) {
    case OutputFormat::XYZ:
      return std::make_unique<outputWriter::XYZWriter>();


    case OutputFormat::VTK:

      return std::make_unique<outputWriter::VTKWriter>();

    case OutputFormat::Checkpoint:
      return std::make_unique<outputWriter::CheckpointWriter>();

    default:
      throw std::runtime_error("Unsupported OutputFormat");
  }
}
