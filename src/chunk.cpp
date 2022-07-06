#include "chunk.hpp"
#include "tile.hpp"
#include "node.hpp"

#include "terrain.hpp"

Chunk::Chunk(int bx, int by, int bz, Terrain * ter_){
    ter = ter_;
    Cx=bx;
    Cy=by;
    Cz=bz;
    for (int x = size * bx; x < size*(1+bx); x++)
    for (int y = size * by; y < size*(1+by); y++)
    for (int z = size * bz; z < size*(1+bz); z++){
        if (ter->can_stand_1(x,y,z)){
            NodeGroup group = NodeGroup(ter->get_tile(x,y,z), 31);
            node_groups.push_back(group);
            ter->add_NodeGroup(&node_groups.back());
        }
    }
    for (NodeGroup& NG : node_groups){
        for (const Tile* tile_main : NG.get_tiles()){
            for (const Tile* tile_adjacent : ter->get_adjacent_Tiles(tile_main, 31)){
                if (NodeGroup* to_add = ter->get_NodeGroup(tile_adjacent)){
                    NG.add_adjacent(to_add, 31);
                }
            }
        }
    }

    auto it = node_groups.begin();
    while (it != node_groups.end()){

        // to merge = get_adjacent_map()
        std::set<NodeGroup*> to_merge;
        for (std::pair<NodeGroup *const, OnePath> other : (it)->get_adjacent_map()){ //! remove nullprt?
            if (other.first != nullptr && contains_nodeGroup(other.first)){
                to_merge.insert(other.first);
            }
        }
        R_merge((*it),to_merge);
        it++;
    }
}

void Chunk::R_merge(NodeGroup &G1, std::set<NodeGroup*>& to_merge){
    if (to_merge.size() == 0){
        return;
    }
    std::set<NodeGroup*> new_merge;
    while(to_merge.size()>0){
        auto G2 = to_merge.begin();
        std::map<NodeGroup *, OnePath> to_add = G1.merge_groups(**(G2));
        delNodeGroup(**G2);
        new_merge.erase(*G2);
        ter->add_NodeGroup(&G1);
        for (std::pair<NodeGroup *const, OnePath> NG : to_add){
            if (contains_nodeGroup(NG.first) && &G1 != NG.first){
                new_merge.insert(NG.first);
            }
        }
        to_merge.erase(G2);
    }
    R_merge(G1, new_merge);
}

//Chunk::~Chunk(){

//}

const std::list<NodeGroup>& Chunk::get_node_groups() const {
    return node_groups;
}
std::list<NodeGroup>& Chunk::get_node_groups() {
    return node_groups;
}

void Chunk::add_nodes_to(std::set<const NodeGroup*>& out) const{
    for (auto it = node_groups.begin(); it != node_groups.end(); it++){
        auto elem = *it;
        out.insert(&elem);  // Ptr to element
    }
}

void Chunk::insert_nodes(std::map<const NodeGroup*,Node<const NodeGroup>>& nodes, std::array<float, 3> sop) const {
    for (const NodeGroup& NG : node_groups){
        nodes.insert(std::make_pair(&NG, Node(&NG, Terrain::get_H_cost( NG.sop(), sop) )));
    }
}

void Chunk::delNodeGroup(NodeGroup &NG){
    // remove form ter. tile to group map
    ter->remove_NodeGroup(&NG);
    for (std::pair<NodeGroup *const, OnePath> &adjacent : NG.get_adjacent_map()){
        adjacent.first->remove_adjacent(&NG);
    }
    node_groups.remove(NG);
}

void Chunk::mergeNodeGroup(NodeGroup &G1, NodeGroup &G2){
    G1.merge_groups(G2);
    delNodeGroup(G2);
    //return out;
    ter->add_NodeGroup(&G1);
}

inline bool Chunk::contains_nodeGroup(NodeGroup* NG){
    return (NG->get_center_x() >= size * Cx && NG->get_center_x() < size*(1+Cx) && 
            NG->get_center_y() >= size * Cy && NG->get_center_y() < size*(1+Cy) &&
            NG->get_center_z() >= size * Cz && NG->get_center_z() < size*(1+Cz));
}