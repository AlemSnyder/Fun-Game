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

private:
    T *tile;
    Node *parent_node;

    float gCost; //Time from start to this node
    float hCost; //Minimum time from this node to end
    float fCost; //Minimum time from start to end through this node
    bool explored;
};

#endif
