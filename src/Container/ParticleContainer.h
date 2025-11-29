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
#include <utility>
#include <vector>

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
class ParticleContainer {
 public:
  /// Type alias for the underlying storage container.
  using storage_type = std::vector<Particle>;
  /// Mutable iterator for particle access.
  using iterator = storage_type::iterator;
  /// Immutable iterator for particle access.
  using const_iterator = storage_type::const_iterator;

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
  [[nodiscard]] std::size_t size() const noexcept;

  /**
   * @brief Checks whether the container is empty.
   * @return True if there are no particles stored, false otherwise.
   */
  [[nodiscard]] bool empty() const noexcept;

  /**
   * @brief Preallocates memory for a given number of particles.
   * This can improve performance by avoiding reallocations during insertion.
   * @param capacity The number of particles to reserve space for.
   */
  void reserve(std::size_t capacity);

  /**
   * @brief Removes all particles from the container.
   *
   * This does not deallocate memory; it simply resets the size to zero.
   */
  void clear() noexcept;

  /**
   * @brief Adds a particle by copying an existing one.
   * @param particle The particle to be copied.
   * @return Reference to the newly added particle.
   */
  Particle &addParticle(const Particle &particle);

  /**
   * @brief Adds a particle by moving it into the container.
   * @param particle The particle to be moved.
   * @return Reference to the newly added particle.
   *
   * This avoids unnecessary copying and is more efficient when the particle
   * will no longer be used by the caller.
   */
  Particle &addParticle(Particle &&particle);

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
  Particle &emplaceParticle(Args &&...args) {
    particles_.emplace_back(std::forward<Args>(args)...);
    return particles_.back();
  }

  /// @brief Returns an iterator to the first particle.
  iterator begin() noexcept;

  /// @brief Returns an iterator past the last particle.
  iterator end() noexcept;

  /// @brief Returns a const iterator to the first particle.
  const_iterator cbegin() const noexcept;

  /// @brief Returns a const iterator past the last particle.
  const_iterator cend() const noexcept;

  /**
   * @brief Iterates over all unique particle pairs (non-const version).
   * @tparam Func Callable type that accepts two Particle references.
   * @param visitor Function or lambda to apply to each particle pair.
   *
   * Calls the provided function for each unique pair `(i, j)` where
   * `i < j`, ensuring that no pair is visited twice.
   */
  template <typename Func>
  void forEachPair(Func &&visitor) {
    for (std::size_t i = 0; i < particles_.size(); ++i) {
      for (std::size_t j = i + 1; j < particles_.size(); ++j) {
        visitor(particles_[i], particles_[j]);
      }
    }
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
  void forEachPair(Func &&visitor) const {
    for (std::size_t i = 0; i < particles_.size(); ++i) {
      for (std::size_t j = i + 1; j < particles_.size(); ++j) {
        visitor(particles_[i], particles_[j]);
      }
    }
  }

 private:
  /// Internal particle storage.
  storage_type particles_;
};
