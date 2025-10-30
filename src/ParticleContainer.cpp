#include "ParticleContainer.h"

#include <utility>

/**
 * @brief Construct a ParticleContainer with reserved capacity.
 *
 * @param reserve Number of particles to reserve space for (optional performance optimization).
 */
ParticleContainer::ParticleContainer(std::size_t reserve) {
  particles_.reserve(reserve);
}

/**
 * @brief Get the current number of particles in the container.
 *
 * @return Number of stored particles.
 */
std::size_t ParticleContainer::size() const noexcept {
  return particles_.size();
}

/**
 * @brief Check whether the container is empty.
 *
 * @return true if no particles are stored, false otherwise.
 */
bool ParticleContainer::empty() const noexcept {
  return particles_.empty();
}

/**
 * @brief Reserve memory for a given number of particles.
 *
 * This can improve performance by avoiding reallocations during insertion.
 * @param capacity Desired capacity to reserve.
 */
void ParticleContainer::reserve(std::size_t capacity) {
  particles_.reserve(capacity);
}

/**
 * @brief Remove all particles from the container.
 */
void ParticleContainer::clear() noexcept {
  particles_.clear();
}

/**
 * @brief Add a new particle by copying.
 *
 * @param particle Particle to copy into the container.
 * @return Reference to the newly added particle.
 */
Particle &ParticleContainer::addParticle(const Particle &particle) {
  particles_.push_back(particle);
  return particles_.back();
}

/**
 * @brief Add a new particle by moving.
 *
 * This uses the move constructor to avoid unnecessary copying.
 * @param particle Particle to move into the container.
 * @return Reference to the newly added particle.
 */
Particle &ParticleContainer::addParticle(Particle &&particle) {
  particles_.push_back(std::move(particle));
  return particles_.back();
}

/**
 * @brief Get an iterator to the beginning of the container.
 */
ParticleContainer::iterator ParticleContainer::begin() noexcept {
  return particles_.begin();
}

/**
 * @brief Get an iterator to the end of the container.
 */
ParticleContainer::iterator ParticleContainer::end() noexcept {
  return particles_.end();
}

/**
 * @brief Get a const iterator to the beginning of the container.
 */
ParticleContainer::const_iterator ParticleContainer::begin() const noexcept {
  return particles_.begin();
}

/**
 * @brief Get a const iterator to the end of the container.
 */
ParticleContainer::const_iterator ParticleContainer::end() const noexcept {
  return particles_.end();
}

/**
 * @brief Get a const iterator to the beginning (read-only).
 *
 * Equivalent to begin() but ensures no modification is possible.
 */
ParticleContainer::const_iterator ParticleContainer::cbegin() const noexcept {
  return particles_.cbegin();
}

/**
 * @brief Get a const iterator to the end (read-only).
 *
 * Equivalent to end() but ensures no modification is possible.
 */
ParticleContainer::const_iterator ParticleContainer::cend() const noexcept {
  return particles_.cend();
}