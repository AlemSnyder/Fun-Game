#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <set>

template<class T>
class Node {  // Used to find paths.
public:
    Node(T *tile, float hc);
    Node();// needs to stay but should not be used
    void init(T *tile, float hc);

    inline bool is_open() { return !this->tile->is_solid(); }
    void explore(Node<T> *parent, float gc);
    void explore();  //

    // if nodes are compared it might be breadth first or A*
    // in these cases the comparison is determined if the nodes is used for
    // A* or for breadth first.
    //inline bool operator<(const Node<T> *other) {
    //    return this->fCost < other->fCost;
    //}
    //inline bool operator>(const Node<T> *other) {
    //    return this->fCost > other->fCost;
    //}
    // Setters

    // Getters
    T *get_tile() { return tile; }
    const T *get_tile() const { return tile; }
    Node<T> *get_parent() { return parent_node; }

    float get_gCost() const { return gCost; } //Time from start to this node
    float get_hCost() const { return hCost; } //Minimum time from this node to end
    float get_fCost() const { return fCost; } //Minimum time from start to end through this node
    bool is_explored() const { return explored; }
    std::set<const T*> get_adjacent(int path_type) const; //The nodes that can be reached from this one
    std::set<const T*> get_adjacent() const; //The nodes that can be reached from this one

private:
    T *tile;
    Node *parent_node;

    float gCost; //Time from start to this node
    float hCost; //Minimum time from this node to end
    float fCost; //Minimum time from start to end through this node
    bool explored;
};

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
    parent_node = nullptr;
    tile = nullptr;
    gCost = 0;
    hCost = 0;
    fCost = gCost + hCost;
    explored = false;
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
template<class T>
std::set<const T*> Node<T>::get_adjacent() const { // explore as start
    return get_tile()->get_adjacent_clear();
}

#endif
