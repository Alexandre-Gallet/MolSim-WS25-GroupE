/**
 * @file ParticleContainer.h
 * @brief Light-weight container wrapper around a particle storage.
 *
 * The ParticleContainer class encapsulates a collection of Particle objects and
 * provides convenient iteration and management utilities. It allows both
 * range-based and pairwise iteration over particles.
 */

#pragma once

#include <cstddef>
#include <array>
#include <functional>
#include <utility>
#include <vector>

#include "Container.h"
#include "Particle.h"

/**
 * @class ParticleContainer
 * @brief Encapsulates particle storage and provides iteration utilities.
 *
 * This class manages a collection of Particle objects using an internal
 * `std::vector<Particle>` as its storage. It provides methods for adding,
 * reserving, and clearing particles as well as iterating over single particles
 * or unique particle pairs.
 *
 * The class ensures that particle management (creation, storage, iteration)
 * remains centralized, which improves maintainability and reduces redundant
 * code across the simulation.
 */
class ParticleContainer : public Container {
 public:
  /// Type alias for the underlying storage container.
  using storage_type = std::vector<Particle>;
  using iterator = Container::iterator;
  using const_iterator = Container::const_iterator;

  /**
   * @brief Default constructor.
   *
   * Constructs an empty container without preallocating any memory.
   */
  ParticleContainer() = default;

  /**
   * @brief Constructs a container with reserved capacity.
   * @param reserve Number of particles to reserve memory for.
   *
   * This avoids frequent reallocations when adding a large number of particles.
   */
  explicit ParticleContainer(std::size_t reserve);

  /**
   * @brief Returns the number of particles currently stored.
   * @return The number of elements in the container.
   */
  [[nodiscard]] auto size() const noexcept -> std::size_t override;

  /**
   * @brief Checks whether the container is empty.
   * @return True if there are no particles stored, false otherwise.
   */
  [[nodiscard]] auto empty() const noexcept -> bool override;

  /**
   * @brief Preallocates memory for a given number of particles.
   * This can improve performance by avoiding reallocations during insertion.
   * @param capacity The number of particles to reserve space for.
   */
  auto reserve(std::size_t capacity) -> void override;

  /**
   * @brief Removes all particles from the container.
   *
   * This does not deallocate memory; it simply resets the size to zero.
   */
  auto clear() noexcept -> void override;

  auto begin() noexcept -> iterator override;
  auto end() noexcept -> iterator override;
  auto begin() const noexcept -> const_iterator override;
  auto end() const noexcept -> const_iterator override;
  auto cbegin() const noexcept -> const_iterator override;
  auto cend() const noexcept -> const_iterator override;

  /**
   * @brief Adds a particle by copying an existing one.
   * @param particle The particle to be copied.
   * @return Reference to the newly added particle.
   */
  auto addParticle(const Particle &particle) -> Particle &;

  /**
   * @brief Adds a particle by moving it into the container.
   * @param particle The particle to be moved.
   * @return Reference to the newly added particle.
   *
   * This avoids unnecessary copying and is more efficient when the particle
   * will no longer be used by the caller.
   */
  auto addParticle(Particle &&particle) -> Particle &;

  /// Add an existing particle reference (Container interface).
  auto addParticle(Particle &particle) -> Particle & override;
  /// Emplace a particle with explicit state (Container interface).
  auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass,
                       int type) -> Particle & override;
  /// Convenience overload that uses the default type.
  auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass)
      -> Particle &;

  /**
   * @brief Constructs and adds a new particle in-place.
   * @tparam Args Argument types for the Particle constructor.
   * @param args Arguments forwarded to the Particle constructor.
   * @return Reference to the newly emplaced particle.
   *
   * This avoids temporary objects and provides the most efficient way to
   * construct particles directly inside the container.
   */
  template <typename... Args>
  auto emplaceParticle(Args &&...args) -> Particle & {
    particles_.emplace_back(std::forward<Args>(args)...);
    return particles_.back();
  }

  /**
   * @brief Iterates over all unique particle pairs (non-const version).
   * @tparam Func Callable type that accepts two Particle references.
   * @param visitor Function or lambda to apply to each particle pair.
   *
   * Calls the provided function for each unique pair `(i, j)` where
   * `i < j`, ensuring that no pair is visited twice.
   */
  template <typename Func>
  void forEachPair(Func visitor) {
    forEachPairImpl(particles_, visitor);
  }

  auto forEachPair(const std::function<void(Particle &, Particle &)> &visitor) -> void override {
    forEachPair<const std::function<void(Particle &, Particle &)> &>(visitor);
  }

  /**
   * @brief Iterates over all unique particle pairs (const version).
   * @tparam Func Callable type that accepts two const Particle references.
   * @param visitor Function or lambda to apply to each particle pair.
   *
   * Same as the non-const version, but guarantees that particles
   * will not be modified.
   */
  template <typename Func>
  void forEachPair(Func visitor) const {
    forEachPairImpl(particles_, visitor);
  }

 private:
  template <typename Func, typename Storage>
  static void forEachPairImpl(Storage &storage, Func &visitor) {
    for (std::size_t i = 0; i < storage.size(); ++i) {
      for (std::size_t j = i + 1; j < storage.size(); ++j) {
        visitor(storage[i], storage[j]);
      }
    }
  }

  /// Internal particle storage.
  storage_type particles_;
};
