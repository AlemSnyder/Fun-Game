#ifndef __CHUNK_HPP__
#define __CHUNK_HPP__

#include "tile.hpp"
#include "node.hpp"
#include "node_group.hpp"
#include "terrain.hpp"
#include <list>
#include <set>

class Terrain;

class Chunk
{
private:
    
    std::list<NodeGroup> NodeGroups;
    Terrain* ter;
    void delNodeGroup(NodeGroup &NG);
    void mergeNodeGroup(NodeGroup &g1, NodeGroup &g2);
    int Cx,Cy,Cz;
    bool contains_nodeGroup(NodeGroup*);
    void R_merge(NodeGroup &g1, std::set<NodeGroup*>& to_merge);
    
public:
    static const int8_t size = 16;
    Chunk(int bx, int by, int bz, Terrain * ter_);
    //~Chunk();
};


#endif