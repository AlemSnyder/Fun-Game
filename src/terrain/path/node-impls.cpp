#include "node.cpp"
#include "node_group.hpp"
#include "../tile.hpp"

namespace terrain {

template class Node<Tile>;
template class Node<NodeGroup>;

template class Node<Tile const>;
template class Node<NodeGroup const>;

} // namespace terrain
