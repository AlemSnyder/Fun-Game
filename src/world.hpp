#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "terrain.hpp"
#include <cstdint>
#include "json/json.h"

class World{
public:
    World();
    World(const char * path);
    World(Json::Value biome_data, int type); // this one is for testing
    World(Json::Value materials_json, Json::Value biome_data);

    Terrain terrain_main;
    void save();

    const std::map<int, const Material>* get_materials() const{
        return &materials;
    }

    void init_materials(Json::Value material_data);

private:
    //Json::Value materials;

    std::map<int, const Material> materials;

    const char * path;
    //const char *name;
    //const char *another_name;


};

#endif
