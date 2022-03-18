#include "node.hpp"
//#include "terrain.hpp"


void Node::init(Tile *tile_, Tile *goal, float hc) {
    parent_node = NULL;
    tile = tile_;
    gCost = 0;
    hCost = hc;//Terrain::get_H_cost(tile, goal);
    fCost = gCost + hCost;
    explored = false;
}

Node::Node(Tile *tile_, Tile *goal, float hc) { init(tile_, goal, hc); }

Node::Node() {
    parent_node = NULL;
    tile = new Tile();
    gCost = 0;
    hCost = 0; 
    fCost = gCost + hCost;
    explored = false;
}

void Node::explore(Node *parent, float gc) {
    this->parent_node = parent;
    gCost = gc;//Terrain::get_G_cost(tile, parent);
    // hCost = Terrain::get_H_cost(tile, goal);
    fCost = gCost + hCost;
    explored = true;
}
void Node::explore() {  // explore for start
    this->parent_node = NULL;
    gCost = 0;  // distance from start should be 0 when this is the start
    // hCost = Terrain::get_H_cost(tile, goal);
    fCost = gCost + hCost;
    explored = true;
}