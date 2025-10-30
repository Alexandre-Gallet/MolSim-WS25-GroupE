// src/MolSim.cpp
#include <iostream>
#include <vector>
#include <list>
#include <array>
#include <string>
#include <cmath>

#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"
#ifdef ENABLE_VTK_OUTPUT
  #include "outputWriter/VTKWriter.h"
#endif

void calculateF();
void calculateX(double dt);
void calculateV(double dt);
void plotParticles(int iteration);

std::vector<Particle> particles;

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Usage: " << argv[0] << " <delta> <end> [inputFile]\n";
    std::cout << "Example (from build dir): ./MolSim 0.014 1000 ../input/eingabe-sonne.txt\n";
    return 1;
  }

  const double delta_t   = std::stod(argv[1]);
  const double end_time  = std::stod(argv[2]);
  const std::string inputFile = (argc >= 4) ? argv[3] : "../input/eingabe-sonne.txt";

  if (delta_t <= 0.0 || end_time <= 0.0) {
    std::cerr << "Invalid args: delta and end must be > 0.\n";
    return 1;
  }

  // --- FileReader: readFile(std::list<Particle>&, char*) ---
  FileReader reader;
  std::list<Particle> plist_in;

  // std::string -> char* (safe)
  std::vector<char> fname(inputFile.begin(), inputFile.end());
  fname.push_back('\0');
  reader.readFile(plist_in, fname.data());

  // list -> vector
  particles.assign(plist_in.begin(), plist_in.end());
  std::cout << "Loaded " << particles.size() << " particles from " << inputFile << "\n";

  for (auto &p : particles) {
    p.setF({0.0, 0.0, 0.0});
    p.setOldF({0.0, 0.0, 0.0});
  }

  calculateF();

  double t = 0.0;
  int iter = 0;
  while (t < end_time) {
    calculateX(delta_t);
    calculateF();
    calculateV(delta_t);

    ++iter;
    if (iter % 10 == 0)
      plotParticles(iter);

    t += delta_t;
  }

  std::cout << "Done.\n";
  return 0;
}

void calculateF() {
  for (auto &p : particles) {
    p.setOldF(p.getF());
    p.setF({0.0, 0.0, 0.0});
  }

  const double eps2 = 1e-12;
  const std::size_t n = particles.size();

  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = i + 1; j < n; ++j) {
      auto &pi = particles[i];
      auto &pj = particles[j];

      const auto xi = pi.getX();
      const auto xj = pj.getX();
      const double mi = pi.getM();
      const double mj = pj.getM();

      std::array<double,3> r{ xj[0]-xi[0], xj[1]-xi[1], xj[2]-xi[2] };
      const double r2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2] + eps2;
      const double inv_r  = 1.0 / std::sqrt(r2);
      const double inv_r3 = inv_r * inv_r * inv_r;

      const double s = (mi * mj) * inv_r3;
      const std::array<double,3> Fij{ s*r[0], s*r[1], s*r[2] };

      auto Fi = pi.getF(); Fi[0] += Fij[0]; Fi[1] += Fij[1]; Fi[2] += Fij[2]; pi.setF(Fi);
      auto Fj = pj.getF(); Fj[0] -= Fij[0]; Fj[1] -= Fij[1]; Fj[2] -= Fij[2]; pj.setF(Fj);
    }
  }
}

// --- Position update ---
void calculateX(double dt) {
  const double dt2h = 0.5 * dt * dt;
  for (auto &p : particles) {
    auto x = p.getX();
    const auto v = p.getV();
    const auto F = p.getF();
    const double m = p.getM();

    x[0] += dt * v[0] + dt2h * (F[0] / m);
    x[1] += dt * v[1] + dt2h * (F[1] / m);
    x[2] += dt * v[2] + dt2h * (F[2] / m);
    p.setX(x);
  }
}

// --- Velocity update ---
void calculateV(double dt) {
  const double dth = 0.5 * dt;
  for (auto &p : particles) {
    auto v  = p.getV();
    const auto Fo = p.getOldF();
    const auto F  = p.getF();
    const double m = p.getM();

    v[0] += dth * ((Fo[0] + F[0]) / m);
    v[1] += dth * ((Fo[1] + F[1]) / m);
    v[2] += dth * ((Fo[2] + F[2]) / m);
    p.setV(v);
  }
}

// --- Output ---
void plotParticles(int iteration) {
  std::string out_name = "MD_viz";
#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  std::list<Particle> plist(particles.begin(), particles.end());
  writer.plotParticles(plist, out_name, iteration);
}
