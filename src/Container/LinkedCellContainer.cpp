
#include "LinkedCellContainer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>

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
  for (auto &per_face : boundary_cells_by_face) {
    per_face.clear();
  }

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
          if (x == 1) boundary_cells_by_face[static_cast<std::size_t>(Face::XMin)].push_back(stored);
          if (x == padded_dims[0] - 2) boundary_cells_by_face[static_cast<std::size_t>(Face::XMax)].push_back(stored);
          if (y == 1) boundary_cells_by_face[static_cast<std::size_t>(Face::YMin)].push_back(stored);
          if (y == padded_dims[1] - 2) boundary_cells_by_face[static_cast<std::size_t>(Face::YMax)].push_back(stored);
          if (z == 1) boundary_cells_by_face[static_cast<std::size_t>(Face::ZMin)].push_back(stored);
          if (z == padded_dims[2] - 2) boundary_cells_by_face[static_cast<std::size_t>(Face::ZMax)].push_back(stored);
        }
      }
    }
  }
}

void LinkedCellContainer::deleteHaloCells() {
  std::unordered_set<Particle *> halo_particles;
  halo_particles.reserve(size());
  for (auto *cell : halo_cells) {
    halo_particles.insert(cell->particles.begin(), cell->particles.end());
    cell->particles.clear();
  }

  owned_particles.erase(
      std::remove_if(owned_particles.begin(), owned_particles.end(),
                     [&](const auto &ptr) -> bool { return halo_particles.find(ptr.get()) != halo_particles.end(); }),
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
  ghost_particles.clear();
  for (auto &cell : cells) {
    cell.particles.clear();
  }

  wrapPeriodicParticles();

  for (auto &p : owned_particles) {
    placeParticle(p.get());
  }

  deleteHaloCells();

  static constexpr std::array<Face, 6> faces{Face::XMin, Face::XMax, Face::YMin, Face::YMax, Face::ZMin, Face::ZMax};

  // Reflecting ghosts: keep as face-based
  for (std::size_t i = 0; i < faces.size(); ++i) {
    if (boundary_conditions.at(i) == BoundaryCondition::Reflecting) {
      createGhostsForFace(faces.at(i));
    }
  }

  // Periodic ghosts: create all required images incl. edges/corners
  createAllPeriodicGhosts();

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
  const auto face_index = static_cast<std::size_t>(face);

  std::vector<Particle *> candidates;

  for (auto *cell : boundary_cells_by_face.at(face_index)) {
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

void LinkedCellContainer::createAllPeriodicGhosts() {
  bool any_periodic = false;
  for (std::size_t i = 0; i < boundary_conditions.size(); ++i) {
    if (boundary_conditions[i] == BoundaryCondition::Periodic) {
      any_periodic = true;
      break;
    }
  }
  if (!any_periodic) return;

  std::vector<Particle *> candidates;
  candidates.reserve(size());
  for (auto *cell : boundary_cells) {
    candidates.insert(candidates.end(), cell->particles.begin(), cell->particles.end());
  }

  // Thickness of the "boundary layer" that needs periodic images.
  // r_cutoff is the physically correct choice: only particles within cutoff
  // of a periodic boundary can interact across it.
  const double layer = r_cutoff;

  for (auto *particle : candidates) {
    const auto pos = particle->getX();

    // For each axis, gather possible shifts we should generate.
    // Always include 0. Add +L or -L depending on which periodic boundary
    // the particle is close to.
    std::array<std::vector<double>, 3> shifts;
    for (int axis = 0; axis < 3; ++axis) {
      shifts[axis].clear();
      shifts[axis].push_back(0.0);

      const double span = domain_size.at(axis);
      if (span <= 0.0) continue;

      const double lower = domain_min.at(axis);
      const double upper = lower + span;

      const auto min_face = static_cast<std::size_t>(axis * 2);
      const auto max_face = static_cast<std::size_t>(axis * 2 + 1);

      const bool per_min = boundary_conditions.at(min_face) == BoundaryCondition::Periodic;
      const bool per_max = boundary_conditions.at(max_face) == BoundaryCondition::Periodic;

      // If particle is within 'layer' of the lower boundary, then the matching
      // periodic image is shifted by +L (from the opposite side).
      if (per_min && pos.at(axis) < lower + layer) {
        shifts[axis].push_back(+span);
      }

      // If particle is within 'layer' of the upper boundary, then the matching
      // periodic image is shifted by -L.
      if (per_max && pos.at(axis) >= upper - layer) {
        shifts[axis].push_back(-span);
      }
    }

    // Generate all non-zero combinations (up to 26 in 3D).
    for (double sx : shifts[0]) {
      for (double sy : shifts[1]) {
        for (double sz : shifts[2]) {
          if (sx == 0.0 && sy == 0.0 && sz == 0.0) continue;

          ghost_particles.push_back(std::make_unique<Particle>(*particle));
          auto *ghost = ghost_particles.back().get();

          auto gpos = ghost->getX();
          gpos[0] += sx;
          gpos[1] += sy;
          gpos[2] += sz;
          ghost->setX(gpos);

          placeParticle(ghost);
        }
      }
    }
  }
}

void LinkedCellContainer::wrapPeriodicParticles() {
  for (auto &p : owned_particles) {
    auto pos = p->getX();
    bool changed = false;

    for (int axis = 0; axis < 3; ++axis) {
      const auto min_face = static_cast<std::size_t>(axis * 2);
      const auto max_face = static_cast<std::size_t>(axis * 2 + 1);
      const double lower = domain_min.at(axis);
      const double upper = lower + domain_size.at(axis);
      const double span = domain_size.at(axis);
      if (span <= 0.0) {
        continue;
      }

      if (pos.at(axis) < lower && boundary_conditions.at(min_face) == BoundaryCondition::Periodic) {
        while (pos.at(axis) < lower) {
          pos.at(axis) += span;
        }
        changed = true;
      } else if (pos.at(axis) >= upper && boundary_conditions.at(max_face) == BoundaryCondition::Periodic) {
        while (pos.at(axis) >= upper) {
          pos.at(axis) -= span;
        }
        changed = true;
      }
    }

    if (changed) {
      p->setX(pos);
    }
  }
}
