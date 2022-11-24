#include "node.hpp"

#include "node_group.hpp"
#include "terrain/tile.hpp"

namespace terrain {

template <class T>
void
Node<T>::explore(Node<T>* parent, float gc) { // explore from parent
    if (explored_) {
        if (g_cost_ > gc) {
            this->parent_node_ = parent;
            g_cost_ = gc;
            f_cost_ = g_cost_ + h_cost_;
        }
    } else {
        this->parent_node_ = parent;
        g_cost_ = gc;
        f_cost_ = g_cost_ + h_cost_;
        explored_ = true;
    }
}

template <class T>
void
Node<T>::explore() { // explore as start
    this->parent_node_ = nullptr;
    g_cost_ = 0; // distance from start should be 0 when this is the start
    f_cost_ = g_cost_ + h_cost_;
    explored_ = true;
}

} // namespace terrain
