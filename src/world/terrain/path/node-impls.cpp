
//! This is an implementations file. Anything not implemented below will lead
//! to linker errors.

#include "../tile.hpp"
#include "node.cpp"
#include "node_group.hpp"
#include "node_wrappers.hpp"

namespace world {

namespace terrain {

namespace path {

template class Node<PositionWrapper>;
template class Node<NodeGroupWrapper>;

template class Node<PositionWrapper const>;
template class Node<NodeGroupWrapper const>;

} // namespace path

} // namespace terrain
} // namespace world
