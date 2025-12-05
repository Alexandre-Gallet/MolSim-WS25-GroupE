/**
 * @file YamlInputReader.h
 * @brief Declaration of the YAML-based input reader.
 */
#pragma once

#include <array>
#include <string>

#include "SimulationConfig.h"
#include "YamlConverter.h"

// Forward declaration to avoid pulling in yaml-cpp in header consumers.
namespace YAML {
class Node;
}

/**
 * @brief Class responsible for reading simulation configuration from a YAML file.
 *
 * This class encapsulates all usage of the yaml-cpp library. It parses a given
 * YAML file and produces a SimulationConfig object.
 */
class YamlInputReader {
 public:
  /**
   * @brief Construct a new YamlInputReader object.
   *
   * @param filename Path to the YAML configuration file.
   */
  explicit YamlInputReader(const std::string &filename);

  /**
   * @brief Parse the YAML configuration file and return a SimulationConfig.
   *
   * On errors (missing sections, invalid types, etc.), this function throws
   * std::runtime_error with a descriptive error message.
   *
   * @return SimulationConfig Parsed configuration.
   */
  SimulationConfig parse() const;

 private:
  /// Path to the YAML file to read.
  std::string filename;

  /// Parse the "simulation" section of the YAML file into the config.
  void parseSimulationSection(const YAML::Node &node, SimulationConfig &cfg) const;

  /// Parse the "output" section of the YAML file into the config.
  void parseOutputSection(const YAML::Node &node, SimulationConfig &cfg) const;

  /// Parse the "cuboids" section of the YAML file into the config.
  void parseCuboidsSection(const YAML::Node &node, SimulationConfig &cfg) const;

  /// parse Disc section in YAML file
  void parseDiscsSection(const YAML::Node &node, SimulationConfig &cfg) const;

  /// parse LinkedCell section in YAML file
  void parseLinkedCellSection(const YAML::Node &node, SimulationConfig &cfg) const;

  /// Helper to parse a 3-element vector of doubles from a YAML sequence.
  std::array<double, 3> parseVec3(const YAML::Node &n, const std::string &fieldName) const;

  /// Helper to parse a 3-element vector of ints from a YAML sequence.
  std::array<int, 3> parseVec3Int(const YAML::Node &n, const std::string &fieldName) const;
};
