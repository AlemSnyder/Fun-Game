
//! This is an implementations file. Anything not implemented below will lead
//! to linker errors.

#include "../tile.hpp"
#include "node.cpp"
#include "node_group.hpp"
#include "node_wrappers.hpp"

namespace terrain {
/*
template class Node<Tile>;
template class Node<NodeGroup>;

template class Node<Tile const>;
template class Node<NodeGroup const>;
*/

template class Node<PositionWrapper>;
template class Node<NodeGroupWrapper>;

template class Node<PositionWrapper const>;
template class Node<NodeGroupWrapper const>;

} // namespace terrain
