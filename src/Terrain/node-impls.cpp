#include "node.cpp"
#include "tile.hpp"
#include "node_group.hpp"

namespace terrain {

template class Node<Tile>;
template class Node<NodeGroup>;

template class Node<Tile const>;
template class Node<NodeGroup const>;

}
