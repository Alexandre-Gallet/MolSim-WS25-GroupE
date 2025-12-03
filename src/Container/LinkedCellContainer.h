/**
 * @file LinkedCellContainer.h
 * @brief Linked-cell particle container for efficient neighbor queries.
 *
 * This container organizes particles into a padded 3D grid (inner, boundary, halo cells)
 * to accelerate pairwise interactions. Particles are owned by the container; cells store
 * pointers into the owned storage.
 */

#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include "Container.h"
#include "Particle.h"
#include "inputReader/Arguments.h"

enum class CellType : uint8_t { Inner, Boundary, Halo };

struct LinkedCell {
  CellType type;                      ///< Cell classification: Inner, Boundary, or Halo.
  std::vector<Particle *> particles;  ///< Pointers to particles located in this cell.
};

/**
 * @class LinkedCellContainer
 * @brief Grid-based particle container implementing the linked-cell algorithm.
 *
 * Cells are laid out in a padded grid (+1 layer per face) to include halos. Each cell
 * holds pointers to particles owned by the container.
 */
class LinkedCellContainer : public Container {
 public:
  using storage_type = std::vector<LinkedCell>;
  using iterator = Container::iterator;
  using const_iterator = Container::const_iterator;

  /**
   * @brief Construct a linked-cell grid for the given domain and cutoff
   * @param r_cutoff Interaction cutoff - defines cell size
   * @param domain_size Domain extents (x,y,z). Origin is (0,0,0)
   */
  LinkedCellContainer(double r_cutoff, const std::array<double, 3> &domain_size);

  template <typename Func>
  void forEachPair(Func visitor);
  auto forEachPair(const std::function<void(Particle &, Particle &)> &visitor) -> void override {
    forEachPair<const std::function<void(Particle &, Particle &)> &>(visitor);
  }

  /**
   * @brief Iterate over all boundary particles (inside domain, adjacent to halos).
   */
  template <typename Func>
  void forEachBoundaryParticle(Func visitor);

  /**
   * @brief Iterate over all halo particles (outside domain, in padded layer).
   */
  template <typename Func>
  void forEachHaloParticle(Func visitor);

  /// Clear all halo particles.
  void deleteHaloCells();

  /// Place a particle into the appropriate cell (inner/boundary/halo).
  auto addParticle(Particle &particle) -> Particle & override;
  /// Emplace a particle with explicit state (Container interface).
  auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass, int type)
      -> Particle & override;
  auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass) -> Particle &;
  /// Total number of particles referenced by the grid.
  [[nodiscard]] auto size() const noexcept -> std::size_t override;
  /// Check if the grid currently holds no particles.
  [[nodiscard]] auto empty() const noexcept -> bool override;
  /// Reserve capacity.
  auto reserve(std::size_t capacity) -> void override;
  /// Remove all particle references from all cells.
  auto clear() noexcept -> void override;
  /// Iteration over owned particles.
  auto begin() -> iterator override;
  auto end() -> iterator override;
  auto begin() const -> const_iterator override;
  auto end() const -> const_iterator override;
  auto cbegin() const -> const_iterator override;
  auto cend() const -> const_iterator override;

  /// Rebuild the cell structure (clears particles and reinitializes metadata).
  void rebuild();

  /// Convert 3D indices to a linear index in the grid.
  static constexpr auto toLinearIndex(std::size_t x, std::size_t y, std::size_t z,
                                      const std::array<std::size_t, 3> &dims) -> std::size_t {
    return x + (dims[0] * (y + (dims[1] * z)));
  }

 private:
  void placeParticle(Particle *particle);
  void initDimensions();
  void initCells();
  void initHalo();

  static constexpr auto computeTotalCells(const std::array<std::size_t, 3> &dims) -> std::size_t {
    return dims[0] * dims[1] * dims[2];
  }

  storage_type cells;
  std::vector<std::unique_ptr<Particle>> owned_particles;  ///< Owned particle storage.
  double r_cutoff;
  std::array<double, 3> cell_dim{};
  std::array<double, 3> domain_size{};
  std::array<std::size_t, 3> cells_per_dim{};
  std::array<std::size_t, 3> padded_dims{};
  std::vector<LinkedCell *> boundary_cells;
  std::vector<LinkedCell *> halo_cells;
};

template <typename Func>
inline void LinkedCellContainer::forEachPair(Func visitor) {
  // Half-stencil covering all 13 forward neighbors to avoid duplicate pair visits.
  static constexpr std::array<std::array<int, 3>, 13> neighbor_offsets{{{{1, 0, 0}},
                                                                        {{1, 1, 0}},
                                                                        {{1, -1, 0}},
                                                                        {{0, 1, 0}},
                                                                        {{1, 0, 1}},
                                                                        {{1, 1, 1}},
                                                                        {{1, -1, 1}},
                                                                        {{0, 1, 1}},
                                                                        {{1, 0, -1}},
                                                                        {{1, 1, -1}},
                                                                        {{1, -1, -1}},
                                                                        {{0, 1, -1}},
                                                                        {{0, 0, 1}}}};

  const auto cells_x = padded_dims.at(0);
  const auto cells_y = padded_dims.at(1);
  const auto cells_z = padded_dims.at(2);
  const auto cells_xy = cells_x * cells_y;

  for (std::size_t linear = 0; linear < cells.size(); ++linear) {
    auto &current_particles = cells[linear].particles;

    for (std::size_t i = 0; i < current_particles.size(); ++i) {
      for (std::size_t j = i + 1; j < current_particles.size(); ++j) {
        visitor(*current_particles[i], *current_particles[j]);
      }
    }

    const int cx = static_cast<int>(linear % cells_x);
    const int cy = static_cast<int>((linear / cells_x) % cells_y);
    const int cz = static_cast<int>(linear / cells_xy);

    for (const auto &offset : neighbor_offsets) {
      const int nx = cx + offset[0];
      const int ny = cy + offset[1];
      const int nz = cz + offset[2];

      if (nx < 0 || ny < 0 || nz < 0) continue;
      if (nx >= static_cast<int>(cells_x) || ny >= static_cast<int>(cells_y) || nz >= static_cast<int>(cells_z))
        continue;

      const std::size_t neighbor_index = toLinearIndex(static_cast<std::size_t>(nx), static_cast<std::size_t>(ny),
                                                       static_cast<std::size_t>(nz), padded_dims);
      auto &neighbor_particles = cells[neighbor_index].particles;
      for (auto *p : current_particles) {
        for (auto *q : neighbor_particles) {
          visitor(*p, *q);
        }
      }
    }
  }
}

template <typename Func>
inline void LinkedCellContainer::forEachBoundaryParticle(Func visitor) {
  for (auto *cell : boundary_cells) {
    for (auto &p : cell->particles) {
      visitor(p);
    }
  }
}

template <typename Func>
inline void LinkedCellContainer::forEachHaloParticle(Func visitor) {
  for (auto *cell : halo_cells) {
    for (auto &p : cell->particles) {
      visitor(p);
    }
  }
}
