#include "node.cpp"
#include "tile.hpp"
#include "node_group.hpp"

template class Node<Tile>;
template class Node<NodeGroup>;

template class Node<Tile const>;
template class Node<NodeGroup const>;
