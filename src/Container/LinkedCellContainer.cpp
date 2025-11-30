//
// Created by darig on 11/20/2025.
//

#include <algorithm>
#include <cmath>
#include "LinkedCellContainer.h"

LinkedCellContainer::LinkedCellContainer() : LinkedCellContainer(1.0, {1.0, 1.0, 1.0}) {}

LinkedCellContainer::LinkedCellContainer(double r_cutoff, const std::array<double, 3>& domain_size)
    : r_cutoff(r_cutoff), domain_size(domain_size) {
  initDimensions();
  initCells();
  initHalo();
}

void LinkedCellContainer::initDimensions() {
  for (int i = 0; i < 3; ++i) {
    if (domain_size.at(i) > 0) {
      cells_per_dim.at(i) = static_cast<std::size_t>(std::ceil(domain_size.at(i) / r_cutoff));
      cells_per_dim.at(i) = std::max<std::size_t>(1, cells_per_dim.at(i));
      cell_dim.at(i) = domain_size.at(i) / static_cast<double>(cells_per_dim.at(i));
    } else {
      cells_per_dim.at(i) = 1;
      cell_dim.at(i) = r_cutoff;
    }
  }

  for (int i = 0; i < 3; ++i) {
    padded_dims.at(i) = cells_per_dim.at(i) + 2;
  }
}

void LinkedCellContainer::initCells() {
  const std::size_t total_cells = computeTotalCells(padded_dims);
  cells.clear();
  cells.reserve(total_cells);

  for (std::size_t z = 0; z < padded_dims[2]; ++z) {
    for (std::size_t y = 0; y < padded_dims[1]; ++y) {
      for (std::size_t x = 0; x < padded_dims[0]; ++x) {
        LinkedCell cell{};
        const bool is_halo = x == 0 || y == 0 || z == 0 || x == padded_dims[0] - 1 ||
                             y == padded_dims[1] - 1 || z == padded_dims[2] - 1;
        const bool is_boundary = !is_halo && (x == 1 || y == 1 || z == 1 || x == padded_dims[0] - 2 ||
                                              y == padded_dims[1] - 2 || z == padded_dims[2] - 2);

        if (is_halo) {
          cell.type = CellType::Halo;
        } else if (is_boundary) {
          cell.type = CellType::Boundary;
        } else {
          cell.type = CellType::Inner;
        }

        cells.push_back(std::move(cell));
      }
    }
  }
}

void LinkedCellContainer::initHalo() {
  halo_cells.clear();
  boundary_cells.clear();
  for (auto &cell : cells) {
    if (cell.type == CellType::Halo) {
      halo_cells.push_back(&cell);
    } else if (cell.type == CellType::Boundary) {
      boundary_cells.push_back(&cell);
    }
  }
}

void LinkedCellContainer::deleteHaloCells() {
  std::vector<Particle*> to_delete;
  to_delete.reserve(size());
  for (auto *cell : halo_cells) {
    to_delete.insert(to_delete.end(), cell->particles.begin(), cell->particles.end());
    cell->particles.clear();
  }

  owned_particles.erase(std::remove_if(owned_particles.begin(), owned_particles.end(),
                                       [&](const auto &ptr) {
                                         return std::find(to_delete.begin(), to_delete.end(), ptr.get()) !=
                                                to_delete.end();
                                       }),
                        owned_particles.end());
}

auto LinkedCellContainer::addParticle(Particle &particle) -> Particle & {
  owned_particles.push_back(std::make_unique<Particle>(particle));
  auto *stored = owned_particles.back().get();
  placeParticle(stored);
  return *stored;
}

auto LinkedCellContainer::emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel,
                                          double mass, int type) -> Particle & {
  owned_particles.push_back(std::make_unique<Particle>(pos, vel, mass, type));
  auto *stored = owned_particles.back().get();
  placeParticle(stored);
  return *stored;
}

auto LinkedCellContainer::emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel,
                                          double mass) -> Particle & {
  return emplaceParticle(pos, vel, mass, 0);
}

auto LinkedCellContainer::size() const noexcept -> std::size_t {
  return owned_particles.size();
}

auto LinkedCellContainer::empty() const noexcept -> bool { return size() == 0; }

auto LinkedCellContainer::reserve(std::size_t capacity) -> void {
  owned_particles.reserve(capacity);
}

auto LinkedCellContainer::clear() noexcept -> void {
  owned_particles.clear();
  for (auto &cell : cells) {
    cell.particles.clear();
  }
}

void LinkedCellContainer::rebuild() {
  for (auto &cell : cells) {
    cell.particles.clear();
  }
  for (auto &p : owned_particles) {
    placeParticle(p.get());
  }
}

auto LinkedCellContainer::begin() -> iterator {
  return {[this](std::size_t idx) { return owned_particles[idx].get(); }, 0};
}

auto LinkedCellContainer::end() -> iterator {
  return {[this](std::size_t idx) { return owned_particles[idx].get(); }, owned_particles.size()};
}

auto LinkedCellContainer::begin() const -> const_iterator {
  return {[this](std::size_t idx) { return owned_particles[idx].get(); }, 0};
}

auto LinkedCellContainer::end() const -> const_iterator {
  return {[this](std::size_t idx) { return owned_particles[idx].get(); }, owned_particles.size()};
}

auto LinkedCellContainer::cbegin() const -> const_iterator { return begin(); }

auto LinkedCellContainer::cend() const -> const_iterator { return end(); }

void LinkedCellContainer::placeParticle(Particle *particle) {
  const auto &pos = particle->getX();

  std::array<std::size_t, 3> idx{};
  for (int i = 0; i < 3; ++i) {
    if (pos.at(i) < 0.0) {
      idx.at(i) = 0;
    } else if (pos.at(i) > domain_size.at(i)) {
      idx.at(i) = padded_dims.at(i) - 1;
    } else {
      const auto raw = static_cast<std::size_t>(pos.at(i) / cell_dim.at(i));
      idx.at(i) = std::min<std::size_t>(raw + 1, padded_dims.at(i) - 2);
    }
  }

  cells[toLinearIndex(idx[0], idx[1], idx[2], padded_dims)].particles.push_back(particle);
}
