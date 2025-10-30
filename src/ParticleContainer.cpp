#include "ParticleContainer.h"

#include <utility>

ParticleContainer::ParticleContainer(std::size_t reserve) { particles_.reserve(reserve); }

std::size_t ParticleContainer::size() const noexcept { return particles_.size(); }

bool ParticleContainer::empty() const noexcept { return particles_.empty(); }

void ParticleContainer::reserve(std::size_t capacity) { particles_.reserve(capacity); } // Can also extend if not enough capacity

void ParticleContainer::clear() noexcept { particles_.clear(); }

Particle &ParticleContainer::addParticle(const Particle &particle) {
  particles_.push_back(particle);
  return particles_.back();
}

Particle &ParticleContainer::addParticle(Particle &&particle) { // Moving variables instead of copying (move constructor)
  particles_.push_back(std::move(particle));
  return particles_.back();
}

ParticleContainer::iterator ParticleContainer::begin() noexcept { return particles_.begin(); }

ParticleContainer::iterator ParticleContainer::end() noexcept { return particles_.end(); }

ParticleContainer::const_iterator ParticleContainer::begin() const noexcept { return particles_.begin(); }

ParticleContainer::const_iterator ParticleContainer::end() const noexcept { return particles_.end(); }

ParticleContainer::const_iterator ParticleContainer::cbegin() const noexcept { return particles_.cbegin(); } // Avoid Writing

ParticleContainer::const_iterator ParticleContainer::cend() const noexcept { return particles_.cend(); }
