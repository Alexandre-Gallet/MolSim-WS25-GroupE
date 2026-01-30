
#include "ParticleProfiling.h"

#include <filesystem>
#include <fstream>

void ParticleProfiling::computeProfiling(const Container &particles, double x_axis, double y_axis, double z_axis,
                                         int n_bins, int iteration) {
  double bin_width = x_axis / n_bins;
  std::vector<double> velocity(n_bins, 0.0);
  std::vector<int> particles_per_bin(n_bins, 0);
  for (const auto &p : particles) {
    if (p.getType() != 1) {
      int bin_index = static_cast<int>(p.getX()[0]) / bin_width;
      particles_per_bin[bin_index]++;
      velocity[bin_index] = velocity[bin_index] + p.getV()[1];
    }
  }
  std::string folder = "profiling";
  std::filesystem::create_directories(folder);

  std::string file_name = folder + "/particle_profiling_" + std::to_string(iteration) + ".csv";
  std::ofstream csv_file(file_name);
  csv_file << "Bin,Average_velocity,Density\n";
  for (int i = 0; i < n_bins; i++) {
    double average_vel = 0.0;
    if (particles_per_bin[i] > 0) {
      average_vel = velocity[i] / particles_per_bin[i];
    }
    double density = particles_per_bin[i] / (bin_width * y_axis * z_axis);
    csv_file << i << ", " << average_vel << ", " << density << "\n";
  }
  csv_file.close();
}
