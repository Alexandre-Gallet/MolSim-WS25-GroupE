#include <yaml-cpp/yaml.h>
#include "Container/ContainerType.h"

namespace YAML {
template<>
struct convert<ContainerType> {
  static bool decode(const Node& node, ContainerType& rhs) {
    if (!node.IsScalar()) return false;

    const auto str = node.as<std::string>();
    rhs = parseContainerType(str);
    return true;
  }
};
template<>
struct convert<BoundaryCondition> {
  static bool decode(const Node& node, BoundaryCondition& rhs) {
    if (!node.IsScalar()) return false;

    const auto str = node.as<std::string>();
    rhs = parseBoundaryCondition(str);
    return true;
  }
};
};