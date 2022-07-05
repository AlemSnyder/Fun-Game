#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "terrain.hpp"
#include "json/json.h"

#include <glm/glm.hpp>
#include <stdint.h>
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

    const Material* get_material(int element_id) const;

    void init_materials(Json::Value material_data);

    void get_mesh_greedy(std::vector<unsigned short> &indices,
					std::vector<glm::vec3> &indexed_vertices,
					std::vector<glm::vec3> &indexed_uvs,
					std::vector<glm::vec3> &indexed_normals) const;

private:
    //Json::Value materials;

    std::map<int, const Material> materials;

    const char * path;
    //const char *name;
    //const char *another_name;


};

#endif // __WORLD_HPP__
