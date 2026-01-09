/**
 * @file LinkedCellContainer.h
 * @brief Linked-cell particle container for efficient neighbor queries with boundary handling.
 *
 * This container organizes particles into a padded 3D grid (inner, boundary, halo cells)
 * to accelerate pairwise interactions. It supports outflow, reflecting, and periodic boundary
 * conditions by maintaining halos and generating ghost particles where needed. Particles are
 * owned by the container; cells store pointers into the owned storage for fast traversal.
 *
 * \image html runtime_per_iter.jpeg
 * \image html runtime_wall_vs_particles.jpeg
 * \image html runtime_time_per_particle.jpeg
 * \image html runtime_wall_vs_particles.jpeg
 */

#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Container.h"
#include "Particle.h"
#include "spdlog/spdlog.h"

enum class Face : uint8_t { XMin = 0, XMax = 1, YMin = 2, YMax = 3, ZMin = 4, ZMax = 5 };
enum class BoundaryCondition : uint8_t { None, Outflow, Reflecting, Periodic };
inline BoundaryCondition parseBoundaryCondition(const std::string &s) {
  if (s == "None" || s == "none") return BoundaryCondition::None;
  if (s == "Outflow" || s == "outflow") return BoundaryCondition::Outflow;
  if (s == "Reflecting" || s == "reflecting") return BoundaryCondition::Reflecting;
  if (s == "Periodic" || s == "periodic") return BoundaryCondition::Periodic;

  SPDLOG_ERROR("Invalid boundary condition: {}", s);
  return BoundaryCondition::None;  // safe fallback
}
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
 * holds pointers to particles owned by the container. Boundary conditions are implemented
 * by rebuilding halos on demand: outflow drops halo particles, reflecting mirrors boundary
 * particles into halos with flipped velocity on the normal axis, and periodic copies
 * boundary particles to the opposite halo shifted by the domain length.
 */
class LinkedCellContainer : public Container {
 public:
  using storage_type = std::vector<LinkedCell>;
  using iterator = Container::iterator;
  using const_iterator = Container::const_iterator;

  /// @brief Construct a minimal 1x1x1 grid with unit cutoff.
  LinkedCellContainer();
  /**
   * @brief Construct a linked-cell grid for the given domain and cutoff.
   * @param r_cutoff Interaction cutoff; defines cell size.
   * @param domain_size Physical domain extents (x,y,z). Origin starts at (0,0,0) unless shifted for thin z-domains.
   */
  LinkedCellContainer(double r_cutoff, const std::array<double, 3> &domain_size);

  /**
   * @brief Configure boundary conditions for each face.
   * @param conditions Order: XMin, XMax, YMin, YMax, ZMin, ZMax.
   *
   * Settings are applied on the next @ref rebuild invocation.
   */
  void setBoundaryConditions(const std::array<BoundaryCondition, 6> &conditions);
  [[nodiscard]] auto getBoundaryConditions() const -> const std::array<BoundaryCondition, 6> &;

  /**
   * @brief Rebuild cell occupancy and halos.
   *
   * Clears cell membership, wraps particles across periodic faces, reassigns particles to
   * cells, removes halo particles for outflow/none, and generates reflecting or periodic
   * ghosts as configured.
   */
  void rebuild();
  /// @brief Clear all halo particles (used by outflow and before ghost creation).
  void deleteHaloCells();

  /// @brief Store a copy of the given particle and place it in the grid.
  auto addParticle(Particle &particle) -> Particle & override;
  /// @brief Emplace a particle with explicit state (Container interface).
  auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass, int type)
      -> Particle & override;
  auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass) -> Particle &;
  /// @brief Total number of owned particles (ghosts excluded).
  [[nodiscard]] auto size() const noexcept -> std::size_t override;
  /// @brief Check if the grid currently holds no particles.
  [[nodiscard]] auto empty() const noexcept -> bool override;
  /// @brief Reserve storage for owned particles.
  auto reserve(std::size_t capacity) -> void override;
  /// @brief Remove all particles and clear cell membership.
  auto clear() noexcept -> void override;

  /// Iteration over owned particles.
  auto begin() -> iterator override;
  auto end() -> iterator override;
  auto begin() const -> const_iterator override;
  auto end() const -> const_iterator override;
  auto cbegin() const -> const_iterator override;
  auto cend() const -> const_iterator override;

  /// Iterate over all unordered pairs.
  template <typename Func>
  void forEachPair(Func visitor);
  auto forEachPair(const std::function<void(Particle &, Particle &)> &visitor) -> void override {
    forEachPair<const std::function<void(Particle &, Particle &)> &>(visitor);
  }
  /**
   * @brief Iterate over all boundary particles (inside domain, adjacent to halos).
   * @param visitor Function called with each boundary particle pointer.
   */
  template <typename Func>
  void forEachBoundaryParticle(Func visitor);
  /**
   * @brief Iterate over all halo particles (outside domain, in padded layer).
   * @param visitor Function called with each halo particle pointer.
   */
  template <typename Func>
  void forEachHaloParticle(Func visitor);

 private:
  void initDimensions();
  void initCells();
  void placeParticle(Particle *particle);
  void createGhostsForFace(Face face);
  void createAllPeriodicGhosts();  ///< Mirror boundary particles into opposite halo for all faces.
  void wrapPeriodicParticles();    ///< Wrap owned particles back into domain for periodic BC.
  [[nodiscard]] auto to3DIndex(std::size_t linear_index) const -> std::array<std::size_t, 3>;
  void logParticleCounts() const;

  static constexpr auto computeTotalCells(const std::array<std::size_t, 3> &dims) -> std::size_t {
    return dims[0] * dims[1] * dims[2];
  }
  /// Convert 3D indices to a linear index in the grid.
  static constexpr auto toLinearIndex(std::size_t x, std::size_t y, std::size_t z,
                                      const std::array<std::size_t, 3> &dims) -> std::size_t {
    return x + (dims[0] * (y + (dims[1] * z)));
  }

  static constexpr int axisFromFace(Face face) {
    switch (face) {
      case Face::XMin:
      case Face::XMax:
        return 0;
      case Face::YMin:
      case Face::YMax:
        return 1;
      case Face::ZMin:
      case Face::ZMax:
        return 2;
    }
    return 0;
  }
  static constexpr auto isUpper(Face face) -> bool {
    return face == Face::XMax || face == Face::YMax || face == Face::ZMax;
  }
  static constexpr Face opposite(Face face) {
    switch (face) {
      case Face::XMin:
        return Face::XMax;
      case Face::XMax:
        return Face::XMin;
      case Face::YMin:
        return Face::YMax;
      case Face::YMax:
        return Face::YMin;
      case Face::ZMin:
        return Face::ZMax;
      case Face::ZMax:
        return Face::ZMin;
    }
    return Face::XMin;
  }

  storage_type cells;
  std::vector<std::unique_ptr<Particle>> owned_particles;  ///< Owned particle storage.
  std::vector<std::unique_ptr<Particle>> ghost_particles;  ///< Ghost particle storage (not counted as owned).
  double r_cutoff;
  std::array<double, 3> cell_dim{};
  std::array<double, 3> domain_size{};
  std::array<double, 3> domain_min{};  ///< Optional shift of the domain origin (used for thin z-domains).
  std::array<std::size_t, 3> cells_per_dim{};
  std::array<std::size_t, 3> padded_dims{};
  std::vector<LinkedCell *> boundary_cells;
  std::array<std::vector<LinkedCell *>, 6> boundary_cells_by_face;
  std::vector<LinkedCell *> halo_cells;
  std::array<BoundaryCondition, 6> boundary_conditions{BoundaryCondition::Outflow, BoundaryCondition::Outflow,
                                                       BoundaryCondition::Outflow, BoundaryCondition::Outflow,
                                                       BoundaryCondition::Outflow, BoundaryCondition::Outflow};
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
