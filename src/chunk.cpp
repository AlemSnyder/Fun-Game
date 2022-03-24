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
            NodeGroup group = NodeGroup(ter->get_tile(x,y,z));
            NodeGroups.push_back(group);
            ter->add_NodeGroup(&NodeGroups.back());
        }
    }
    for (NodeGroup& NG : NodeGroups){
        for (const Tile* tile_main : NG.get_tiles()){
            for (const Tile* tile_adjacent : ter->get_adjacent_Tiles(tile_main, 31)){
                if (NodeGroup* to_add = ter->get_NodeGroup(tile_adjacent)){
                    NG.add_adjacent(to_add);
                }
            }
        }
    }

    std::list<NodeGroup>::iterator it = NodeGroups.begin(); //it != NodeGroups.end(); it++
    //auto test = NodeGroups[4];
    while (it != NodeGroups.end()){

        // to merge = get_adjacent()
        std::set<NodeGroup*> to_merge;
        for (auto other : (it)->get_adjacent()){
            if (other != nullptr && contains_nodeGroup(other)){
                to_merge.insert(other);
            }
        }
        R_merge((*it),to_merge);
        //auto other = (it)->get_adjacent().begin();

        //std::list<NodeGroup*>::const_iterator it_next = (it)->get_adjacent().begin();//it != NodeGroups.end(); it++
        //mergeNodeGroup((*it), *(it_next)));
        it++;
    }

    //for (std::list<NodeGroup>::iterator it = NodeGroups.begin(); it != NodeGroups.end(); it++){
    //    while ((it)->get_adjacent().size()>0){
    //        NodeGroup* other = (it)->get_adjacent().back();
    //        if (contains_nodeGroup(other)){
    //            mergeNodeGroup((*it), *other);
    //        }
    //    }
    //}
}

void Chunk::R_merge(NodeGroup &G1, std::set<NodeGroup*>& to_merge){
    if (to_merge.size() == 0){
        return;
    }
    std::set<NodeGroup*> new_merge;
    //for (NodeGroup* G2 : to_merge){
    while(to_merge.size()>0){
        auto G2 = to_merge.begin();
        //auto test = **(G2);
        std::set<NodeGroup*> to_add = G1.merge_groups(**(G2));
        delNodeGroup(**G2);
        new_merge.erase(*G2);
        ter->add_NodeGroup(&G1);
        for (NodeGroup* NG : to_add){
            if (contains_nodeGroup(NG) && &G1 != NG){
                new_merge.insert(NG);
            }
        }
        to_merge.erase(G2);
    }
    R_merge(G1, new_merge);
}

//Chunk::~Chunk(){

//}

void Chunk::delNodeGroup(NodeGroup &NG){
    // remove form ter. tile to group map
    ter->remove_NodeGroup(&NG);
    for (NodeGroup* adjacent : NG.get_adjacent()){
        adjacent->remove_adjacent(&NG);
    }
    NodeGroups.remove(NG);
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