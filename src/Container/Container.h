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

template <typename Value>
class ParticleIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::remove_const_t<Value>;
  using difference_type = std::ptrdiff_t;
  using pointer = Value *;
  using reference = Value &;

  ParticleIterator() = default;
  ParticleIterator(std::function<pointer(std::size_t)> getter, std::size_t index)
      : getter_(std::move(getter)), index_(index) {}

  reference operator*() const { return *getter_(index_); }
  pointer operator->() const { return getter_(index_); }

  ParticleIterator &operator++() {
    ++index_;
    return *this;
  }
  ParticleIterator operator++(int) {
    ParticleIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  friend bool operator==(const ParticleIterator &lhs, const ParticleIterator &rhs) { return lhs.index_ == rhs.index_; }
  friend bool operator!=(const ParticleIterator &lhs, const ParticleIterator &rhs) { return !(lhs == rhs); }

 private:
  std::function<pointer(std::size_t)> getter_;
  std::size_t index_{0};
};

class Container {
 public:
  using iterator = ParticleIterator<Particle>;
  using const_iterator = ParticleIterator<const Particle>;

  virtual ~Container() = default;

  [[nodiscard]] virtual auto size() const noexcept -> std::size_t = 0;
  [[nodiscard]] virtual auto empty() const noexcept -> bool = 0;
  virtual auto reserve(std::size_t capacity) -> void = 0;
  virtual auto clear() noexcept -> void = 0;

  virtual auto begin() -> iterator = 0;
  virtual auto end() -> iterator = 0;
  virtual auto begin() const -> const_iterator = 0;
  virtual auto end() const -> const_iterator = 0;
  virtual auto cbegin() const -> const_iterator = 0;
  virtual auto cend() const -> const_iterator = 0;

  /// Add a particle to the container and return a reference to the stored particle.
  virtual auto addParticle(Particle &particle) -> Particle & = 0;
  /// Emplace a particle with explicit state.
  virtual auto emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass,
                               int type) -> Particle & = 0;

  /// Iterate all unordered particle pairs.
  virtual auto forEachPair(const std::function<void(Particle &, Particle &)> &visitor) -> void = 0;
};
