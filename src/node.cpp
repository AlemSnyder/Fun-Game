//#include "node.hpp"
//#include "terrain.hpp"
/*
template<class T>
void Node<T>::init(T *tile_, float hc) {
    parent_node = NULL;
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
    parent_node = NULL;
    tile = nullptr;
    gCost = 0;
    hCost = 0; 
    fCost = gCost + hCost;
    explored = false;
}

template<class T>
void Node<T>::explore(Node<T> *parent, float gc) {
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
void Node<T>::explore() {  // explore for start
    this->parent_node = NULL;
    gCost = 0;  // distance from start should be 0 when this is the start
    // hCost = Terrain::get_H_cost(tile, goal);
    fCost = gCost + hCost;
    explored = true;
}*/