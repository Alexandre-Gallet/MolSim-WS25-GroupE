//
// Created by darig on 11/20/2025.
//

#include "LinkedCellContainer.h"

#include <algorithm>
#include <cmath>
#include <iostream>

LinkedCellContainer::LinkedCellContainer() : LinkedCellContainer(1.0, {1.0, 1.0, 1.0}) {}

LinkedCellContainer::LinkedCellContainer(double r_cutoff, const std::array<double, 3> &domain_size)
    : r_cutoff(r_cutoff), domain_size(domain_size) {
  domain_min.fill(0.0);
  // If the domain collapses to a single cell along z, center the cell on z = 0
  // so generated particles do not sit directly on the lower wall.
  if (std::abs(domain_size.at(2) - 1.0) < 1e-9) {
    domain_min.at(2) = -0.5 * domain_size.at(2);
  }
  initDimensions();
  initCells();
}

void LinkedCellContainer::setBoundaryConditions(const std::array<BoundaryCondition, 6> &conditions) {
  boundary_conditions = conditions;
}

auto LinkedCellContainer::getBoundaryConditions() const -> const std::array<BoundaryCondition, 6> & {
  return boundary_conditions;
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
  halo_cells.clear();
  boundary_cells.clear();

  for (std::size_t z = 0; z < padded_dims[2]; ++z) {
    for (std::size_t y = 0; y < padded_dims[1]; ++y) {
      for (std::size_t x = 0; x < padded_dims[0]; ++x) {
        LinkedCell cell{};
        const bool is_halo =
            x == 0 || y == 0 || z == 0 || x == padded_dims[0] - 1 || y == padded_dims[1] - 1 || z == padded_dims[2] - 1;
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
        auto *stored = &cells.back();
        if (stored->type == CellType::Halo) {
          halo_cells.push_back(stored);
        } else if (stored->type == CellType::Boundary) {
          boundary_cells.push_back(stored);
        }
      }
    }
  }
}

void LinkedCellContainer::deleteHaloCells() {
  std::vector<Particle *> to_delete;
  to_delete.reserve(size());
  for (auto *cell : halo_cells) {
    to_delete.insert(to_delete.end(), cell->particles.begin(), cell->particles.end());
    cell->particles.clear();
  }

  owned_particles.erase(std::remove_if(owned_particles.begin(), owned_particles.end(),
                                       [&](const auto &ptr) -> bool {
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

auto LinkedCellContainer::size() const noexcept -> std::size_t { return owned_particles.size(); }

auto LinkedCellContainer::empty() const noexcept -> bool { return size() == 0; }

auto LinkedCellContainer::reserve(std::size_t capacity) -> void { owned_particles.reserve(capacity); }

auto LinkedCellContainer::clear() noexcept -> void {
  owned_particles.clear();
  for (auto &cell : cells) {
    cell.particles.clear();
  }
}

void LinkedCellContainer::rebuild() {
  ghost_particles.clear();  // drop ghosts from previous step
  for (auto &cell : cells) {
    cell.particles.clear();
  }

  for (auto &p : owned_particles) {
    placeParticle(p.get());
  }

  deleteHaloCells();

  static constexpr std::array<Face, 6> faces{Face::XMin, Face::XMax, Face::YMin, Face::YMax, Face::ZMin, Face::ZMax};
  for (std::size_t i = 0; i < faces.size(); ++i) {
    if (boundary_conditions.at(i) == BoundaryCondition::Reflecting) {
      createGhostsForFace(faces.at(i));
    }
  }

  // logParticleCounts();
}

auto LinkedCellContainer::begin() -> iterator {
  return {[this](std::size_t idx) -> Particle * { return owned_particles[idx].get(); }, 0};
}

auto LinkedCellContainer::end() -> iterator {
  return {[this](std::size_t idx) -> Particle * { return owned_particles[idx].get(); }, owned_particles.size()};
}

auto LinkedCellContainer::begin() const -> const_iterator {
  return {[this](std::size_t idx) -> const Particle * { return owned_particles[idx].get(); }, 0};
}

auto LinkedCellContainer::end() const -> const_iterator {
  return {[this](std::size_t idx) -> const Particle * { return owned_particles[idx].get(); }, owned_particles.size()};
}

auto LinkedCellContainer::cbegin() const -> const_iterator { return begin(); }

auto LinkedCellContainer::cend() const -> const_iterator { return end(); }

void LinkedCellContainer::placeParticle(Particle *particle) {
  const auto &pos = particle->getX();

  std::array<std::size_t, 3> idx{};
  for (int i = 0; i < 3; ++i) {
    const double shifted = pos.at(i) - domain_min.at(i);
    if (shifted < 0.0) {
      idx.at(i) = 0;
    } else if (shifted > domain_size.at(i)) {
      idx.at(i) = padded_dims.at(i) - 1;
    } else {
      const auto raw = static_cast<std::size_t>(shifted / cell_dim.at(i));
      idx.at(i) = std::min<std::size_t>(raw + 1, padded_dims.at(i) - 2);
    }
  }

  cells[toLinearIndex(idx[0], idx[1], idx[2], padded_dims)].particles.push_back(particle);
}

auto LinkedCellContainer::to3DIndex(std::size_t linear_index) const -> std::array<std::size_t, 3> {
  std::array<std::size_t, 3> coords{};
  coords[0] = linear_index % padded_dims[0];
  coords[1] = (linear_index / padded_dims[0]) % padded_dims[1];
  coords[2] = linear_index / (padded_dims[0] * padded_dims[1]);
  return coords;
}

void LinkedCellContainer::logParticleCounts() const {
  std::size_t halo = 0;
  std::size_t boundary = 0;
  std::size_t inner = 0;

  for (const auto &cell : cells) {
    switch (cell.type) {
      case CellType::Halo:
        halo += cell.particles.size();
        break;
      case CellType::Boundary:
        boundary += cell.particles.size();
        break;
      case CellType::Inner:
        inner += cell.particles.size();
        break;
    }
  }

  std::cout << "Particle counts - inner: " << inner << ", boundary: " << boundary << ", halo: " << halo << "\n";
}

void LinkedCellContainer::createGhostsForFace(Face face) {
  const auto bc = boundary_conditions.at(static_cast<std::size_t>(face));
  if (bc != BoundaryCondition::Reflecting) {
    return;
  }

  const auto axis = axisFromFace(face);
  const bool upper = isUpper(face);
  const auto boundary_coord = upper ? padded_dims.at(axis) - 2 : 1;

  std::vector<Particle *> candidates;

  // Collect particles first to avoid mutating cell storage while iterating it.
  for (auto *cell : boundary_cells) {
    const auto linear_index = static_cast<std::size_t>(cell - cells.data());
    const auto coords = to3DIndex(linear_index);
    if (coords.at(axis) != boundary_coord) {
      continue;
    }
    candidates.insert(candidates.end(), cell->particles.begin(), cell->particles.end());
  }

  for (auto *particle : candidates) {
    ghost_particles.push_back(std::make_unique<Particle>(*particle));
    auto *ghost = ghost_particles.back().get();

    auto ghost_pos = ghost->getX();
    auto ghost_vel = ghost->getV();

    const double lower_bound = domain_min.at(axis);
    const double upper_bound = lower_bound + domain_size.at(axis);

    if (upper) {
      ghost_pos.at(axis) = upper_bound + (upper_bound - ghost_pos.at(axis));
    } else {
      ghost_pos.at(axis) = lower_bound - (ghost_pos.at(axis) - lower_bound);
    }
    ghost_vel.at(axis) = -ghost_vel.at(axis);

    ghost->setX(ghost_pos);
    ghost->setV(ghost_vel);
    placeParticle(ghost);
  }
}
