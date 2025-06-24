#include "node_wrappers.hpp"

namespace terrain {

bool
NodeGroupWrapper::contains(const TerrainOffset3 position) const {
    const auto tiles = nodegroup_->get_tiles();
    const auto it = tiles.find(position);
    return (it != tiles.end());
}

} // namespace terrain
