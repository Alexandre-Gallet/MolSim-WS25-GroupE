/**
 * @file MembraneGenerator.cpp
 */
#include "MembraneGenerator.h"

#include <cmath>

std::vector<Particle *> MembraneGenerator::generate(Container &container, const MembraneConfig &cfg) {
  const int nx = cfg.n[0];
  const int ny = cfg.n[1];
  const int nz = cfg.n[2];

  const std::size_t total = static_cast<std::size_t>(nx) * static_cast<std::size_t>(ny) * static_cast<std::size_t>(nz);
  std::vector<Particle *> grid(total, nullptr);

  container.reserve(container.size() + total);

  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        std::array<double, 3> pos = {cfg.origin[0] + static_cast<double>(i) * cfg.h,
                                     cfg.origin[1] + static_cast<double>(j) * cfg.h,
                                     cfg.origin[2] + static_cast<double>(k) * cfg.h};
        auto &p = container.emplaceParticle(pos, cfg.baseVelocity, cfg.mass, 0);
        p.clearNeighbors();
        grid[linearIndex(i, j, k, cfg.n)] = &p;
      }
    }
  }

  const double diag_rest = std::sqrt(2.0) * cfg.r0;
  auto addNeighborPair = [&](int i0, int j0, int k0, int i1, int j1, int k1, double rest) {
    auto *a = particleAt(grid, cfg.n, i0, j0, k0);
    auto *b = particleAt(grid, cfg.n, i1, j1, k1);
    if (a && b) {
      a->addNeighbor(b, rest);
      b->addNeighbor(a, rest);
    }
  };

  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        if (i + 1 < nx) {
          addNeighborPair(i, j, k, i + 1, j, k, cfg.r0);
        }
        if (j + 1 < ny) {
          addNeighborPair(i, j, k, i, j + 1, k, cfg.r0);
        }
        if (i + 1 < nx && j + 1 < ny) {
          addNeighborPair(i, j, k, i + 1, j + 1, k, diag_rest);
        }
        if (i + 1 < nx && j - 1 >= 0) {
          addNeighborPair(i, j, k, i + 1, j - 1, k, diag_rest);
        }
      }
    }
  }

  return grid;
}

Particle *MembraneGenerator::particleAt(const std::vector<Particle *> &grid, const std::array<int, 3> &dims, int i,
                                        int j, int k) {
  if (i < 0 || j < 0 || k < 0 || i >= dims[0] || j >= dims[1] || k >= dims[2]) {
    return nullptr;
  }
  return grid[linearIndex(i, j, k, dims)];
}

std::size_t MembraneGenerator::linearIndex(int i, int j, int k, const std::array<int, 3> &dims) {
  return static_cast<std::size_t>(i) + static_cast<std::size_t>(dims[0]) *
                                          (static_cast<std::size_t>(j) + static_cast<std::size_t>(dims[1]) *
                                                                           static_cast<std::size_t>(k));
}
