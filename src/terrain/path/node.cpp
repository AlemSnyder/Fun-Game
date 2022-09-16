#include "node.hpp"

#include "node_group.hpp"
#include "../tile.hpp"

namespace terrain {

template <class T>
void
Node<T>::explore(Node<T>* parent, float gc) { // explore from parent
    if (explored_) {
        if (gCost_ > gc) {
            this->parent_node_ = parent;
            gCost_ = gc;
            fCost_ = gCost_ + hCost_;
        }
    } else {
        this->parent_node_ = parent;
        gCost_ = gc;
        fCost_ = gCost_ + hCost_;
        explored_ = true;
    }
}

template <class T>
void
Node<T>::explore() { // explore as start
    this->parent_node_ = nullptr;
    gCost_ = 0; // distance from start should be 0 when this is the start
    fCost_ = gCost_ + hCost_;
    explored_ = true;
}

} // namespace terrain
