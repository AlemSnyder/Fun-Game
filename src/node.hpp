#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <set>

template<class T>
class Node {  // Used to find paths.
public:
    Node(T *tile, float hc);
    Node();// needs to stay but should not be used
    void init(T *tile, float hc);

    //inline bool is_open() { return !this->tile->is_solid(); } this should always be true
    void explore(Node<T> *parent, float gc);
    void explore();  // Explore as if this is the starting position.

    T *get_tile() { return tile; }
    const T *get_tile() const { return tile; }
    Node<T> *get_parent() { return parent_node; }

    float get_current_cots() const { return gCost; } //Time from start to this node
    float get_predicted_continue_cots() const { return hCost; } //Minimum time from this node to end
    float get_total_predicted_cost() const { return fCost; } //Minimum time from start to end through this node
    
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


#endif
