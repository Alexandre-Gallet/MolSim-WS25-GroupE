/**
 * @file YamlInputReader.cpp
 * @brief Implementation of the YAML-based input reader.
 */
#include "YamlInputReader.h"

#include <yaml-cpp/yaml.h>

#include <array>
#include <sstream>
#include <stdexcept>

#include "Simulation/SimulationType.h"
#include "outputWriter/OutputFormat.h"

// Forward declarations of existing helper functions.

SimulationType parseSimType(const std::string &s);
OutputFormat parse_output(const std::string &s);

YamlInputReader::YamlInputReader(const std::string &filename) : filename(filename) {}

SimulationConfig YamlInputReader::parse() const {
  YAML::Node root;
  try {
    root = YAML::LoadFile(filename);
  } catch (const std::exception &e) {
    std::ostringstream oss;
    oss << "Error loading YAML file '" << filename << "': " << e.what();
    throw std::runtime_error(oss.str());
  }

  SimulationConfig cfg;

  // --- simulation section ---
  if (!root["simulation"]) {
    throw std::runtime_error("YAML error: missing 'simulation' section");
  }
  parseSimulationSection(root["simulation"], cfg);

  // --- output section ---
  if (!root["output"]) {
    throw std::runtime_error("YAML error: missing 'output' section");
  }
  parseOutputSection(root["output"], cfg);

  // --- cuboids section (optional) ---
  if (root["cuboids"]) {
    parseCuboidsSection(root["cuboids"], cfg);
  }

  // --- discs section (optional) ---
  if (root["discs"]) {
    parseDiscsSection(root["discs"], cfg);
  }

  // --- types section (optional Lennard-Jones overrides) ---
  if (root["types"]) {
    parseTypesSection(root["types"], cfg);
  }

  // --- linked cell section (optional) ---
  if (root["linkedCell"]) {
    parseLinkedCellSection(root["linkedCell"], cfg);
  }

  // --- thermostat section ---
  if (root["thermostat"]) {
    parseThermostatSection(root["thermostat"], cfg);
  }

  return cfg;
}

// Parsing of simulation section in input yml file

void YamlInputReader::parseSimulationSection(const YAML::Node &n, SimulationConfig &cfg) const {
  if (!n["sim_type"]) {
    throw std::runtime_error("YAML error: simulation.sim_type is required");
  }
  if (!n["t_start"] || !n["t_end"] || !n["delta_t"]) {
    throw std::runtime_error("YAML error: simulation.t_start, t_end and delta_t are required");
  }
  if (!n["output_format"]) {
    throw std::runtime_error("YAML error: simulation.output_format is required");
  }

  const std::string typeStr = n["sim_type"].as<std::string>();
  const std::string formatStr = n["output_format"].as<std::string>();

  cfg.sim_type = parseSimType(typeStr);
  cfg.t_start = n["t_start"].as<double>();
  cfg.t_end = n["t_end"].as<double>();
  cfg.delta_t = n["delta_t"].as<double>();
  cfg.output_format = parse_output(formatStr);
  if (n["epsilon"]) {
    cfg.lj_epsilon = n["epsilon"].as<double>();
  }
  if (n["sigma"]) {
    cfg.lj_sigma = n["sigma"].as<double>();
  }

  if (n["checkpoint_file"]) {
    cfg.restart_from_checkpoint = true;
    cfg.checkpoint_file = n["checkpoint_file"].as<std::string>();
  }
  if (n["gravity"]) {
    cfg.gravity = parseVec3(n["gravity"], "gravity");
  }

  if (cfg.t_start > cfg.t_end) {
    throw std::runtime_error("YAML error: simulation.t_start must be <= simulation.t_end");
  }
  if (cfg.delta_t <= 0.0) {
    throw std::runtime_error("YAML error: simulation.delta_t must be > 0");
  }
}

// Parsing of output Section

void YamlInputReader::parseOutputSection(const YAML::Node &n, SimulationConfig &cfg) const {
  if (!n["write_frequency"]) {
    throw std::runtime_error("YAML error: output.write_frequency is required");
  }

  cfg.write_frequency = n["write_frequency"].as<int>();

  if (cfg.write_frequency <= 0) {
    throw std::runtime_error("YAML error: output.write_frequency must be > 0");
  }
}

// Parsing Cuboids section

// Parsing Cuboids section

void YamlInputReader::parseCuboidsSection(const YAML::Node &n, SimulationConfig &cfg) const {
  // Allow null or empty cuboids
  if (!n || n.IsNull()) {
    return;  // no cuboids defined
  }

  if (!n.IsSequence()) {
    throw std::runtime_error("YAML error: 'cuboids' must be a sequence or empty");
  }

  for (const auto &node : n) {
    Cuboid c;

    auto origin = parseVec3(node["origin"], "origin");
    c.origin = {origin[0], origin[1], origin[2]};

    auto num = parseVec3Int(node["numPerDim"], "numPerDim");
    c.numPerDim = {num[0], num[1], num[2]};

    auto vel = parseVec3(node["baseVelocity"], "baseVelocity");
    c.baseVelocity = {vel[0], vel[1], vel[2]};

    if (!node["h"]) {
      throw std::runtime_error("YAML error: cuboid.h is required");
    }
    if (!node["mass"]) {
      throw std::runtime_error("YAML error: cuboid.mass is required");
    }

    c.h = node["h"].as<double>();
    c.mass = node["mass"].as<double>();

    if (node["type"]) {
      c.type = node["type"].as<int>();
    }
    if (node["brownianMean"]) {
      c.brownianMean = node["brownianMean"].as<double>();
    }
    cfg.cuboids.push_back(c);
  }

  if (cfg.cuboids.empty()) {
    // throw std::runtime_error("YAML error: 'cuboids' must contain at least one cuboid");
  }
}

// Parsing Discs section

void YamlInputReader::parseDiscsSection(const YAML::Node &n, SimulationConfig &cfg) const {
  // Allow null or empty discs
  if (!n || n.IsNull()) {
    return;  // no discs defined
  }
  if (!n.IsSequence()) throw std::runtime_error("YAML error: 'discs' must be a sequence");

  for (const auto &node : n) {
    Disc d;

    d.center = parseVec3(node["center"], "center");
    d.radiusCells = node["radiusCells"].as<int>();
    d.hDisc = node["hDisc"].as<double>();
    d.mass = node["mass"].as<double>();
    d.baseVelocity = parseVec3(node["baseVelocityDisc"], "baseVelocityDisc");

    if (node["typeDisc"]) d.typeDisc = node["typeDisc"].as<int>();

    cfg.discs.push_back(d);
  }
}
// Parsing Linked Cell section

void YamlInputReader::parseLinkedCellSection(const YAML::Node &n, SimulationConfig &cfg) const {
  if (!n.IsSequence() || n.size() != 1)
    throw std::runtime_error("YAML error: 'linkedCell' must contain exactly one element");

  const auto &node = n[0];

  // containerType (stored as list in YAML, take first entry)
  if (!node["containerType"] || !node["containerType"].IsSequence())
    throw std::runtime_error("YAML error: linkedCell.containerType must be a sequence");
  cfg.containerType = node["containerType"][0].as<ContainerType>();

  // domain size
  cfg.domainSize = parseVec3(node["domainSize"], "domainSize");

  // rCutoff
  cfg.rCutoff = node["rCutoff"].as<double>();

  // boundary conditions
  if (!node["boundaryConditions"] || !node["boundaryConditions"].IsSequence() || node["boundaryConditions"].size() != 6)
    throw std::runtime_error("YAML error: linkedCell.boundaryConditions must have 6 items");

  for (int i = 0; i < 6; ++i) {
    cfg.boundaryConditions[i] = node["boundaryConditions"][i].as<BoundaryCondition>();
  }
}

void YamlInputReader::parseThermostatSection(const YAML::Node &n, SimulationConfig &cfg) const {
  if (!n || n.IsNull()) {
    return;
  }

  cfg.thermostat.enable_thermostat = true;

  // Required parameters
  if (!n["t_init"] || !n["n_thermostat"] || !n["dimensions"] || !n["brownian_motion"]) {
    throw std::runtime_error("YAML error: t_init, n_thermostat, dimensions, brownian_motion are required parameters!");
  }
  cfg.thermostat.t_init = n["t_init"].as<double>();
  cfg.thermostat.n_thermostat = n["n_thermostat"].as<size_t>();
  cfg.thermostat.dimensions = n["dimensions"].as<size_t>();
  cfg.thermostat.brownian_motion = n["brownian_motion"].as<bool>();

  // Optional parameters
  if (n["t_target"]) {
    cfg.thermostat.t_target = n["t_target"].as<double>();
  } else {
    cfg.thermostat.t_target = cfg.thermostat.t_init;
  }
  if (n["delta_t"]) {
    cfg.thermostat.delta_t = n["delta_t"].as<double>();
  } else {
    cfg.thermostat.delta_t = std::numeric_limits<double>::infinity();
  }
}

std::array<double, 3> YamlInputReader::parseVec3(const YAML::Node &n, const std::string &fieldName) const {
  if (!n || !n.IsSequence() || n.size() != 3) {
    std::ostringstream oss;
    oss << "YAML error: '" << fieldName << "' must be a list of 3 numbers";
    throw std::runtime_error(oss.str());
  }
  return {n[0].as<double>(), n[1].as<double>(), n[2].as<double>()};
}

std::array<int, 3> YamlInputReader::parseVec3Int(const YAML::Node &n, const std::string &fieldName) const {
  if (!n || !n.IsSequence() || n.size() != 3) {
    std::ostringstream oss;
    oss << "YAML error: '" << fieldName << "' must be a list of 3 integers";
    throw std::runtime_error(oss.str());
  }
  return {n[0].as<int>(), n[1].as<int>(), n[2].as<int>()};
}

void YamlInputReader::parseTypesSection(const YAML::Node &n, SimulationConfig &cfg) const {
  if (!n.IsSequence()) throw std::runtime_error("YAML error: 'types' must be a sequence");

  for (const auto &node : n) {
    if (!node["id"] || !node["epsilon"] || !node["sigma"]) {
      throw std::runtime_error("YAML error: types entries require 'id', 'epsilon', and 'sigma'");
    }
    LJTypeParams params;
    params.type = node["id"].as<int>();
    params.epsilon = node["epsilon"].as<double>();
    params.sigma = node["sigma"].as<double>();
    cfg.lj_types.push_back(params);
  }
}
