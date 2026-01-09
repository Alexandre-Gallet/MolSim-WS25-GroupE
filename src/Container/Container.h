/**
 * @file Container.h
 * @brief Abstract base interface for particle containers.
 */
#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>

#include "Particle.h"

/**
 * @class ParticleIteratorImpl
 * @brief A generic iterator implementation for particles.
 * * This class implements a forward iterator that uses a functional getter to access particles.
 * This abstraction allows different container types to provide a unified iteration interface
 * @tparam Value The type being iterated over
 */
template <typename Value>
class ParticleIteratorImpl {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::remove_const_t<Value>;
  using difference_type = std::ptrdiff_t;
  using pointer = Value *;
  using reference = Value &;

  /** @brief Default constructor */
  ParticleIteratorImpl() = default;
  /**
   * @brief Constructs an iterator at a specific position.
   * @param getter A functional wrapper that returns a pointer to a particle given an index.
   * @param index The starting index for this iterator.
   */
  ParticleIteratorImpl(std::function<pointer(std::size_t)> getter, std::size_t index)
      : getter_(std::move(getter)), index_(index) {}

  /**
   * @brief Dereference operator.
   * @return Reference to the particle at the current index
   */
  reference operator*() const { return *getter_(index_); }
  /**
   * @brief Member access operator
   * @return Pointer to the particle at the current index
   */
  pointer operator->() const { return getter_(index_); }

  /**
   * @brief Prefix increment
   * @return Reference to the updated iterator
   */
  ParticleIteratorImpl &operator++() {
    ++index_;
    return *this;
  }
  /**
   * @brief Postfix increment
   * @return The iterator state before the increment
   */
  ParticleIteratorImpl operator++(int) {
    ParticleIteratorImpl tmp(*this);
    ++(*this);
    return tmp;
  }

  /** @brief Checks equality based on the index */
  friend bool operator==(const ParticleIteratorImpl &lhs, const ParticleIteratorImpl &rhs) {
    return lhs.index_ == rhs.index_;
  }
  /** @brief Checks inequality based on the index */
  friend bool operator!=(const ParticleIteratorImpl &lhs, const ParticleIteratorImpl &rhs) { return !(lhs == rhs); }

 private:
  std::function<pointer(std::size_t)> getter_;
  std::size_t index_{0};
};

using ParticleIterator = ParticleIteratorImpl<Particle>;
using ConstParticleIterator = ParticleIteratorImpl<const Particle>;

/**
 * @class Container
 * @brief Abstract base class for particle storage, defines the interface for particle management
 */
class Container {
 public:
  using iterator = ParticleIterator;
  using const_iterator = ConstParticleIterator;

  Container() = default;
  virtual ~Container() = default;

  /** @brief Returns the number of particles in the container */
  [[nodiscard]] virtual auto size() const noexcept -> std::size_t = 0;
  /** @brief Returns true if the container has no particles */
  [[nodiscard]] virtual auto empty() const noexcept -> bool = 0;
  /**
   * @brief Pre-allocates memory to hold at least @p capacity particles
   * @param capacity Number of particles to reserve space for
   */
  virtual auto reserve(std::size_t capacity) -> void = 0;
  /** @brief Removes all particles from the container. */
  virtual auto clear() noexcept -> void = 0;

  /** @name Iterators
   * Standard iterator support for for-loops.
   */
  virtual auto begin() -> iterator = 0;
  virtual auto end() -> iterator = 0;
  virtual auto begin() const -> const_iterator = 0;
  virtual auto end() const -> const_iterator = 0;
  virtual auto cbegin() const -> const_iterator = 0;
  virtual auto cend() const -> const_iterator = 0;

  /**
   * @brief Adds a copy of an existing particle to the container
   * @param particle The particle to be added
   * @return A reference to the particle stored inside the container
   */
  virtual auto addParticle(Particle &particle) -> Particle & = 0;
  /**
   * @brief Constructs a particle directly in the container
   * @param pos Position coordinates
   * @param vel Velocity vector
   * @param mass Mass of the particle
   * @param type Type of the particle
   * @return A reference to the newly created particle
   */
  virtual auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass,
                               int type) -> Particle & = 0;

  /**
   * @brief Executes a function on every unique unordered pair of particles in the container
   * @param visitor A function accepting references to two particles
   */
  virtual auto forEachPair(const std::function<void(Particle &, Particle &)> &visitor) -> void = 0;
};
