/**
 *@file WriterFactory.h
 */
#pragma once

#include <memory>
#include "outputWriter/OutputFormat.h"
#include "outputWriter/OutputWriter.h"
/**
 * Factory class for creating a writer of the specified format
 */
class WriterFactory {
  public:
  /**
   * @param format file format for writing output
   * @return pointer to a writer of the specified format
   */
  static std::unique_ptr<outputWriter::OutputWriter> createWriter(OutputFormat format);
};
