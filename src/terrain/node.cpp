#include "node.hpp"

#include "tile.hpp"
#include "node_group.hpp"

namespace terrain {

template<class T>
void Node<T>::init(T *tile_, float hc) {
    parent_node = nullptr;
    tile = tile_;
    gCost = 0;
    hCost = hc;
    fCost = gCost + hCost;
    explored = false;
}

template<class T>
Node<T>::Node(T *tile_, float hc) { init(tile_, hc); }

template<class T>
Node<T>::Node() {
    init(nullptr, 0);
}

template<class T>
void Node<T>::explore(Node<T> *parent, float gc) { // explore from parent
    if (explored){
        if (gCost > gc){
            this->parent_node = parent;
            gCost = gc;
            fCost = gCost + hCost;
        }
    } else{
        this->parent_node = parent;
        gCost = gc;
        fCost = gCost + hCost;
        explored = true;
    }
}

template<class T>
void Node<T>::explore() { // explore as start
    this->parent_node = nullptr;
    gCost = 0;  // distance from start should be 0 when this is the start
    fCost = gCost + hCost;
    explored = true;
}

template<class T>
std::set<const T*> Node<T>::get_adjacent(int path_type) const { // explore as start
    return get_tile()->get_adjacent_clear(path_type);
}

}
