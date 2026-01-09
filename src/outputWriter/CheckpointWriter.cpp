#include "CheckpointWriter.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
std::string makeFilename(const std::string &base, int iteration) {
  std::ostringstream oss;
  oss << base << "_" << iteration << ".state";
  return oss.str();
}
}  // namespace

namespace outputWriter {

void CheckpointWriter::plotParticles(Container &particles, const std::string &filename, int iteration) {
  const auto out_name = makeFilename(filename, iteration);
  std::ofstream out(out_name);
  if (!out.is_open()) {
    SPDLOG_ERROR("Failed to open checkpoint file '{}' for writing.", out_name);
    return;
  }

  out << "# Phase space checkpoint\n";
  out << "STATE " << particles.size() << "\n";
  out << std::setprecision(17);

  for (auto &p : particles) {
    const auto &x = p.getX();
    const auto &v = p.getV();
    const auto &f = p.getF();
    const auto &old_f = p.getOldF();
    out << x[0] << " " << x[1] << " " << x[2] << " ";
    out << v[0] << " " << v[1] << " " << v[2] << " ";
    out << f[0] << " " << f[1] << " " << f[2] << " ";
    out << old_f[0] << " " << old_f[1] << " " << old_f[2] << " ";
    out << p.getM() << " " << p.getType() << "\n";
  }

  SPDLOG_INFO("Wrote checkpoint with {} particles to '{}'.", particles.size(), out_name);
}

}  // namespace outputWriter
