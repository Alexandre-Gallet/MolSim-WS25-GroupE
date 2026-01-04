/**
 * @file CheckpointReader.cpp
 * @brief Implementation of checkpoint reader for restarting simulations.
 */

#include "CheckpointReader.h"

#include <array>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
bool isStateHeader(const std::string &line) {
  std::istringstream iss(line);
  std::string token;
  iss >> token;
  return token == "STATE";
}
}  // namespace

namespace inputReader {

void CheckpointReader::readFile(Container &particles, const std::string &filename) {
  std::ifstream input(filename);
  if (!input.is_open()) {
    throw std::runtime_error("Could not open checkpoint file: " + filename);
  }

  particles.clear();

  std::string line;
  // Skip comments/empty lines
  while (std::getline(input, line)) {
    if (line.empty() || line[0] == '#') continue;
    break;
  }

  if (!isStateHeader(line)) {
    throw std::runtime_error("Checkpoint file '" + filename + "' missing STATE header");
  }

  std::istringstream header(line);
  std::string keyword;
  std::size_t count = 0;
  header >> keyword >> count;

  for (std::size_t i = 0; i < count; ++i) {
    if (!std::getline(input, line)) {
      throw std::runtime_error("Checkpoint file '" + filename + "' truncated while reading particles");
    }
    if (line.empty() || line[0] == '#') {
      --i;
      continue;
    }

    std::istringstream data(line);
    std::array<double, 3> x{};
    std::array<double, 3> v{};
    std::array<double, 3> f{};
    std::array<double, 3> old_f{};
    double m = 0.0;
    int type = 0;

    for (auto &val : x) data >> val;
    for (auto &val : v) data >> val;
    for (auto &val : f) data >> val;
    for (auto &val : old_f) data >> val;
    data >> m >> type;

    Particle p{x, v, m, type};
    p.setF(f);
    p.setOldF(old_f);
    particles.addParticle(p);
  }
}

}  // namespace inputReader
