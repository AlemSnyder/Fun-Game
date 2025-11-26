#include "node_wrappers.hpp"

namespace world {

namespace terrain {

namespace path {

bool
NodeGroupWrapper::contains(const TerrainOffset3 position) const {
    const auto tiles = nodegroup_->get_tiles();
    const auto it = tiles.find(position);
    return (it != tiles.end());
}

} // namespace path

} // namespace terrain

} // namespace world
