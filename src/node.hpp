#include "tile.hpp"
//#include "terrain.hpp"

#ifndef __NODE_HPP__
#define __NODE_HPP__

class Node {  // Used to find paths.
   public:
    Node(Tile *tile, float hc);  //
    Node();                        // needs to stay but should not be used
    void init(Tile *tile, float hc);

    inline bool is_open() { return !this->tile->is_solid(); }
    void explore(Node *parent, float gc);
    void explore();  //

    inline bool operator<(const Node *other) {
        return this->fCost < other->fCost;
    }
    inline bool operator>(const Node *other) {
        return this->fCost > other->fCost;
    }
    // Setters

    // Getters
    Tile *get_tile() { return tile; }
    const Tile *get_tile() const { return tile; }
    Node *get_parent() { return parent_node; }

    float get_gCost() const { return gCost; }
    float get_hCost() const { return hCost; }
    float get_fCost() const { return fCost; }
    bool is_explored() const { return explored; }

   private:
    Tile *tile;
    Node *parent_node;

    float gCost; //Time from start to this node
    float hCost; //Minimum time from this node to end
    float fCost; //Minimum time from start to end through this node
    bool explored;
};

#endif
//inline bool operator<(const Node &node1, const Node &node2) {
//    return node1 < node2;
//}