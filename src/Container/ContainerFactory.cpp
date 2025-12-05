#include "ContainerFactory.h"

#include "LinkedCellContainer.h"
#include "ParticleContainer.h"

namespace ContainerFactory {
auto createContainer(SimulationConfig &cfg) -> std::unique_ptr<Container> {
  switch (cfg.containerType) {
    case ContainerType::Cell:
      return std::make_unique<LinkedCellContainer>(cfg.rCutoff, cfg.domainSize);
    case ContainerType::Particle:
      return std::make_unique<ParticleContainer>();
  }
}
}  // namespace ContainerFactory
