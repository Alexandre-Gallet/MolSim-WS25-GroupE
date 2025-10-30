/**
 * @file ParticleContainer.h
 * @brief Light-weight container wrapper around a particle storage.
 */

#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "Particle.h"

/**
 * Encapsulates particle storage and provides convenient iteration utilities.
 *
 * A simple wrapper over `std::vector<Particle>` that keeps particle management
 * responsibilities in a single location. It offers both range-based iteration
 * and helper methods for iterating unique particle pairs.
 */
class ParticleContainer {
 public:
  using storage_type = std::vector<Particle>;
  using iterator = storage_type::iterator;
  using const_iterator = storage_type::const_iterator;

  ParticleContainer() = default;
  explicit ParticleContainer(std::size_t reserve);

  [[nodiscard]] std::size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept;

  void reserve(std::size_t capacity);
  void clear() noexcept;

  Particle &addParticle(const Particle &particle);
  Particle &addParticle(Particle &&particle);

  template <typename... Args>
  Particle &emplaceParticle(Args &&...args) {
    particles_.emplace_back(std::forward<Args>(args)...);
    return particles_.back();
  }

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  template <typename Func>
  void forEachPair(Func &&visitor) {
    for (std::size_t i = 0; i < particles_.size(); ++i) {
      for (std::size_t j = i + 1; j < particles_.size(); ++j) {
        visitor(particles_[i], particles_[j]);
      }
    }
  }

  template <typename Func>
  void forEachPair(Func &&visitor) const {
    for (std::size_t i = 0; i < particles_.size(); ++i) {
      for (std::size_t j = i + 1; j < particles_.size(); ++j) {
        visitor(particles_[i], particles_[j]);
      }
    }
  }

 private:
  storage_type particles_;
};
