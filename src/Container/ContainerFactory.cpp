#include "ContainerFactory.h"

#include "Container.h"
#include "LinkedCellContainer.h"
#include "ParticleContainer.h"

namespace ContainerFactory {
auto createContainer(Arguments &args) -> std::unique_ptr<Container> {
  switch (args.cont_type) {
    case ContainerType::Cell:
      return std::make_unique<LinkedCellContainer>(args.r_cutoff, args.domain_size);
    case ContainerType::Particle:
      return std::make_unique<ParticleContainer>();
    default:
      // already checked in parseType, shouldn't be reached

      return std::make_unique<ParticleContainer>();
  }
}
}  // namespace ContainerFactory
