#include "LinkedCellContainer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

namespace {
struct ShiftTableEntry {
  std::array<std::array<int, 3>, 26> shifts{};
  int count = 0;
};

auto buildShiftTable() -> std::array<ShiftTableEntry, 64> {
  std::array<ShiftTableEntry, 64> table{};
  for (std::size_t key = 0; key < table.size(); ++key) {
    int axis_shifts[3][3] = {};
    int axis_counts[3] = {1, 1, 1};
    for (int axis = 0; axis < 3; ++axis) {
      const int mask = static_cast<int>((key >> (axis * 2)) & 0x3);
      axis_shifts[axis][0] = 0;
      if (mask & 0x1) {
        axis_shifts[axis][axis_counts[axis]++] = +1;
      }
      if (mask & 0x2) {
        axis_shifts[axis][axis_counts[axis]++] = -1;
      }
    }

    auto &entry = table[key];
    for (int ix = 0; ix < axis_counts[0]; ++ix) {
      for (int iy = 0; iy < axis_counts[1]; ++iy) {
        for (int iz = 0; iz < axis_counts[2]; ++iz) {
          const int sx = axis_shifts[0][ix];
          const int sy = axis_shifts[1][iy];
          const int sz = axis_shifts[2][iz];
          if (sx == 0 && sy == 0 && sz == 0) continue;
          entry.shifts[entry.count++] = {sx, sy, sz};
        }
      }
    }
  }
  return table;
}

const std::array<ShiftTableEntry, 64> &shiftTable() {
  static const std::array<ShiftTableEntry, 64> table = buildShiftTable();
  return table;
}
}  // namespace

LinkedCellContainer::LinkedCellContainer() : LinkedCellContainer(1.0, {1.0, 1.0, 1.0}) {}

LinkedCellContainer::LinkedCellContainer(double r_cutoff, const std::array<double, 3> &domain_size)
    : r_cutoff(r_cutoff), domain_size(domain_size) {
  domain_min.fill(0.0);
  // If the domain collapses to a single cell along z, center the cell on z = 0
  // so generated particles do not sit directly on the lower wall.
  if (std::abs(domain_size.at(2) - 1.0) < 1e-9) {
    domain_min.at(2) = -0.5 * domain_size.at(2);
  }
  for (int i = 0; i < 3; ++i) {
    domain_max.at(i) = domain_min.at(i) + domain_size.at(i);
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
  for (auto &per_face : periodic_cell_indices_by_face) {
    per_face.clear();
  }
  periodic_candidate_cell_indices.clear();

  std::vector<char> periodic_candidate_marks(total_cells, 0);
  const double layer = r_cutoff;

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

        const std::size_t linear_index = cells.size();
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

          const double cell_min_x = domain_min.at(0) + static_cast<double>(x - 1) * cell_dim.at(0);
          const double cell_max_x = cell_min_x + cell_dim.at(0);
          const double cell_min_y = domain_min.at(1) + static_cast<double>(y - 1) * cell_dim.at(1);
          const double cell_max_y = cell_min_y + cell_dim.at(1);
          const double cell_min_z = domain_min.at(2) + static_cast<double>(z - 1) * cell_dim.at(2);
          const double cell_max_z = cell_min_z + cell_dim.at(2);

          const bool near_x_min = cell_min_x < domain_min.at(0) + layer;
          const bool near_x_max = cell_max_x > domain_max.at(0) - layer;
          const bool near_y_min = cell_min_y < domain_min.at(1) + layer;
          const bool near_y_max = cell_max_y > domain_max.at(1) - layer;
          const bool near_z_min = cell_min_z < domain_min.at(2) + layer;
          const bool near_z_max = cell_max_z > domain_max.at(2) - layer;

          if (near_x_min) periodic_cell_indices_by_face[static_cast<std::size_t>(Face::XMin)].push_back(linear_index);
          if (near_x_max) periodic_cell_indices_by_face[static_cast<std::size_t>(Face::XMax)].push_back(linear_index);
          if (near_y_min) periodic_cell_indices_by_face[static_cast<std::size_t>(Face::YMin)].push_back(linear_index);
          if (near_y_max) periodic_cell_indices_by_face[static_cast<std::size_t>(Face::YMax)].push_back(linear_index);
          if (near_z_min) periodic_cell_indices_by_face[static_cast<std::size_t>(Face::ZMin)].push_back(linear_index);
          if (near_z_max) periodic_cell_indices_by_face[static_cast<std::size_t>(Face::ZMax)].push_back(linear_index);

          if ((near_x_min || near_x_max || near_y_min || near_y_max || near_z_min || near_z_max) &&
              !periodic_candidate_marks[linear_index]) {
            periodic_candidate_marks[linear_index] = 1;
            periodic_candidate_cell_indices.push_back(linear_index);
          }
        }
      }
    }
  }
}

void LinkedCellContainer::deleteHaloCells() {
  if (owned_epoch.size() != owned_particles.size()) {
    owned_epoch.resize(owned_particles.size(), 0);
  }

  if (++owned_epoch_counter == 0) {
    std::fill(owned_epoch.begin(), owned_epoch.end(), 0);
    owned_epoch_counter = 1;
  }

  for (auto *cell : halo_cells) {
    for (auto *particle : cell->particles) {
      const std::size_t idx = particle->owned_index_;
      if (idx < owned_particles.size() && owned_particles[idx].get() == particle) {
        owned_epoch[idx] = owned_epoch_counter;
      }
    }
    cell->particles.clear();
  }

  std::size_t write = 0;
  for (std::size_t read = 0; read < owned_particles.size(); ++read) {
    if (owned_epoch[read] == owned_epoch_counter) {
      continue;
    }
    if (write != read) {
      owned_particles[write] = std::move(owned_particles[read]);
      owned_epoch[write] = owned_epoch[read];
    }
    owned_particles[write]->owned_index_ = static_cast<uint32_t>(write);
    ++write;
  }

  owned_particles.resize(write);
  owned_epoch.resize(write);
}

auto LinkedCellContainer::addParticle(Particle &particle) -> Particle & {
  owned_particles.push_back(std::make_unique<Particle>(particle));
  auto *stored = owned_particles.back().get();
  stored->owned_index_ = static_cast<uint32_t>(owned_particles.size() - 1);
  owned_epoch.push_back(0);
  placeParticle(stored);
  return *stored;
}

auto LinkedCellContainer::emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel,
                                          double mass, int type) -> Particle & {
  owned_particles.push_back(std::make_unique<Particle>(pos, vel, mass, type));
  auto *stored = owned_particles.back().get();
  stored->owned_index_ = static_cast<uint32_t>(owned_particles.size() - 1);
  owned_epoch.push_back(0);
  placeParticle(stored);
  return *stored;
}

auto LinkedCellContainer::emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel,
                                          double mass) -> Particle & {
  return emplaceParticle(pos, vel, mass, 0);
}

auto LinkedCellContainer::size() const noexcept -> std::size_t { return owned_particles.size(); }

auto LinkedCellContainer::empty() const noexcept -> bool { return size() == 0; }

auto LinkedCellContainer::reserve(std::size_t capacity) -> void {
  owned_particles.reserve(capacity);
  owned_epoch.reserve(capacity);
}

auto LinkedCellContainer::clear() noexcept -> void {
  owned_particles.clear();
  owned_epoch.clear();
  owned_epoch_counter = 0;
  for (int axis = 0; axis < 3; ++axis) {
    owned_x[axis].clear();
    owned_v[axis].clear();
  }
  for (auto &cell : cells) {
    cell.particles.clear();
  }
}

void LinkedCellContainer::clearDynamicState() {
  ghost_pool.clear();
  for (auto &cell : cells) {
    cell.particles.clear();
  }
}

void LinkedCellContainer::updateSoaCache() {
  const std::size_t count = owned_particles.size();
  for (int axis = 0; axis < 3; ++axis) {
    owned_x[axis].resize(count);
    owned_v[axis].resize(count);
  }

  for (std::size_t i = 0; i < count; ++i) {
    const auto &pos = owned_particles[i]->getX();
    const auto &vel = owned_particles[i]->getV();
    for (int axis = 0; axis < 3; ++axis) {
      owned_x[axis][i] = pos[axis];
      owned_v[axis][i] = vel[axis];
    }
  }
}

void LinkedCellContainer::placeOwnedParticles() {
  for (auto &p : owned_particles) {
    placeParticle(p.get());
  }
}

void LinkedCellContainer::rebuild() {
  clearDynamicState();

  wrapPeriodicParticles();
  updateSoaCache();

  placeOwnedParticles();

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
  const auto linear_index = linearIndexForPosition(particle->getX());
  placeParticleAtIndex(particle, linear_index);
}

void LinkedCellContainer::placeParticleAtIndex(Particle *particle, std::size_t linear_index) {
  cells[linear_index].particles.push_back(particle);
}

auto LinkedCellContainer::linearIndexForPosition(const std::array<double, 3> &pos) const -> std::size_t {
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
  return toLinearIndex(idx[0], idx[1], idx[2], padded_dims);
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
  std::size_t candidate_count = 0;
  for (auto *cell : boundary_cells_by_face.at(face_index)) {
    candidate_count += cell->particles.size();
  }
  candidates.reserve(candidate_count);

  for (auto *cell : boundary_cells_by_face.at(face_index)) {
    candidates.insert(candidates.end(), cell->particles.begin(), cell->particles.end());
  }

  for (auto *particle : candidates) {
    ghost_pool.emplace_back(*particle);
    auto *ghost = &ghost_pool.back();

    auto ghost_pos = ghost->getX();
    auto ghost_vel = ghost->getV();

    const double lower_bound = domain_min.at(axis);
    const double upper_bound = domain_max.at(axis);

    if (upper) {
      ghost_pos.at(axis) = upper_bound + (upper_bound - ghost_pos.at(axis));
    } else {
      ghost_pos.at(axis) = lower_bound - (ghost_pos.at(axis) - lower_bound);
    }
    ghost_vel.at(axis) = -ghost_vel.at(axis);

    ghost->setX(ghost_pos);
    ghost->setV(ghost_vel);
    const auto linear_index = linearIndexForPosition(ghost_pos);
    placeParticleAtIndex(ghost, linear_index);
  }
}

void LinkedCellContainer::createAllPeriodicGhosts() {
  std::array<std::size_t, 6> periodic_faces{};
  std::size_t periodic_face_count = 0;
  for (std::size_t i = 0; i < boundary_conditions.size(); ++i) {
    if (boundary_conditions[i] == BoundaryCondition::Periodic) {
      periodic_faces[periodic_face_count++] = i;
    }
  }
  if (periodic_face_count == 0) return;

  const std::vector<std::size_t> *candidate_cells = &periodic_candidate_cell_indices;
  if (periodic_face_count == 1) {
    candidate_cells = &periodic_cell_indices_by_face[periodic_faces[0]];
  }
  if (candidate_cells->empty()) return;

  std::vector<Particle *> candidates;
  std::size_t candidate_count = 0;
  for (auto idx : *candidate_cells) {
    candidate_count += cells[idx].particles.size();
  }
  candidates.reserve(candidate_count);
  for (auto idx : *candidate_cells) {
    auto &cell = cells[idx];
    candidates.insert(candidates.end(), cell.particles.begin(), cell.particles.end());
  }

  // Thickness of the "boundary layer" that needs periodic images.
  // r_cutoff is the physically correct choice: only particles within cutoff
  // of a periodic boundary can interact across it.
  const double layer = r_cutoff;

  for (auto *particle : candidates) {
    const auto pos = particle->getX();

    uint8_t key = 0;
    double spans[3] = {domain_size.at(0), domain_size.at(1), domain_size.at(2)};
    for (int axis = 0; axis < 3; ++axis) {
      const double span = spans[axis];
      if (span <= 0.0) continue;

      const double lower = domain_min.at(axis);
      const double upper = domain_max.at(axis);

      const auto min_face = static_cast<std::size_t>(axis * 2);
      const auto max_face = static_cast<std::size_t>(axis * 2 + 1);

      const bool per_min = boundary_conditions.at(min_face) == BoundaryCondition::Periodic;
      const bool per_max = boundary_conditions.at(max_face) == BoundaryCondition::Periodic;

      uint8_t mask = 0;
      if (per_min && pos.at(axis) < lower + layer) {
        mask |= 0x1;
      }
      if (per_max && pos.at(axis) >= upper - layer) {
        mask |= 0x2;
      }

      key |= static_cast<uint8_t>(mask << (axis * 2));
    }

    if (key == 0) {
      continue;
    }

    const auto &entry = shiftTable()[key];
    for (int i = 0; i < entry.count; ++i) {
      const auto &shift = entry.shifts[i];
      const double sx = static_cast<double>(shift[0]) * spans[0];
      const double sy = static_cast<double>(shift[1]) * spans[1];
      const double sz = static_cast<double>(shift[2]) * spans[2];

      ghost_pool.emplace_back(*particle);
      auto *ghost = &ghost_pool.back();

      auto gpos = ghost->getX();
      gpos[0] += sx;
      gpos[1] += sy;
      gpos[2] += sz;
      ghost->setX(gpos);

      const auto linear_index = linearIndexForPosition(gpos);
      placeParticleAtIndex(ghost, linear_index);
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
      const double upper = domain_max.at(axis);
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
