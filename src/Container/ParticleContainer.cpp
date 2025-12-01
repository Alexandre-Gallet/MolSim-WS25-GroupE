#include "ParticleContainer.h"

#include <array>
#include <utility>

ParticleContainer::ParticleContainer(std::size_t reserve) { particles_.reserve(reserve); }

/**
 * @brief Get the current number of particles in the container.
 *
 * @return Number of stored particles.
 */
auto ParticleContainer::size() const noexcept -> std::size_t { return particles_.size(); }

/**
 * @brief Check whether the container is empty.
 *
 * @return true if no particles are stored, false otherwise.
 */
auto ParticleContainer::empty() const noexcept -> bool { return particles_.empty(); }

auto ParticleContainer::reserve(std::size_t capacity) -> void { particles_.reserve(capacity); }

/**
 * @brief Remove all particles from the container.
 */
auto ParticleContainer::clear() noexcept -> void { particles_.clear(); }

auto ParticleContainer::addParticle(const Particle &particle) -> Particle & {
  particles_.push_back(particle);
  return particles_.back();
}

auto ParticleContainer::addParticle(Particle &&particle) -> Particle & {
  particles_.push_back(std::move(particle));
  return particles_.back();
}

auto ParticleContainer::emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass,
                                        int type) -> Particle & {
  particles_.emplace_back(pos, vel, mass, type);
  return particles_.back();
}

auto ParticleContainer::emplaceParticle(const std::array<double, 3> &pos, const std::array<double, 3> &vel, double mass)
    -> Particle & {
  return emplaceParticle(pos, vel, mass, 0);
}

auto ParticleContainer::addParticle(Particle &particle) -> Particle & {
  return addParticle(static_cast<const Particle &>(particle));
}

auto ParticleContainer::begin() noexcept -> iterator {
  return {[this](std::size_t idx) { return &particles_[idx]; }, 0};
}

auto ParticleContainer::end() noexcept -> iterator {
  return {[this](std::size_t idx) { return &particles_[idx]; }, particles_.size()};
}

auto ParticleContainer::begin() const noexcept -> const_iterator {
  return {[this](std::size_t idx) { return &particles_[idx]; }, 0};
}

auto ParticleContainer::end() const noexcept -> const_iterator {
  return {[this](std::size_t idx) { return &particles_[idx]; }, particles_.size()};
}

auto ParticleContainer::cbegin() const noexcept -> const_iterator { return begin(); }

auto ParticleContainer::cend() const noexcept -> const_iterator { return end(); }
