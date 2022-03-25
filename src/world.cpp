#include "world.hpp"
#include "terrain.hpp"
#include <fstream>
#include <stdint.h>

void World::save(){
    terrain_main.qb_save(path);
}

void World::init_materials(Json::Value material_data){
    for (auto element_it = material_data.begin(); element_it != material_data.end(); element_it++){
        
        Material mat;
        mat.element = element_it.key().asCString();
        //std::cout << mat.element << "<- that is the element\n";
        mat.element_id = (*element_it)["id"].asInt();
        mat.solid = (*element_it)["solid"].asBool();
        mat.speed_multiplier = (*element_it)["speed"].asInt();
        //std::cout << "to colors 23\n";
        //for (auto subtype_it = (*element_it)["colors"].begin(); subtype_it != (*element_it)["colors"].end(); subtype_it++ ){
        for (unsigned int i = 0; i < (*element_it)["colors"].size(); i++){
            //std::cout << "start loop\n";
            const char * string = (*element_it)["colors"][i]["name"].asCString();
            //std::cout << "as string\n";
            //std::cout << (*subtype_it).asCString() << " color as string\n";
            uint32_t color = std::stoll((*element_it)["colors"][i]["hex"].asCString(), 0, 16); //materials_json[element_it]["colors"][subtype_it].asInt();
            //std::cout << color << " color as int\n";
            mat.color.push_back(std::make_pair(string, color));
        }
        materials.insert( std::make_pair(mat.element_id, mat));
    }
}

World::World(){
    Json::Value materials_json;
    std::ifstream materials_file("../data/materials.json", std::ifstream::in);
    materials_file >> materials_json;

    init_materials(materials_json);

    std::vector<int> grass_grad_data;
    for (unsigned int i=0; i<materials_json["Dirt"]["Gradient"]["levels"].size(); i++){
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt());
    }

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;

    std::cout << "start of terrain\n";
    terrain_main = Terrain();

}

World::World(const char * path){
    Json::Value materials_json;
    std::ifstream materials_file("../data/materials.json", std::ifstream::in);
    materials_file >> materials_json;

    init_materials(materials_json);

    std::vector<int> grass_grad_data;
    for (unsigned int i=0; i<materials_json["Dirt"]["Gradient"]["levels"].size(); i++){
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt());
    }

    std::cout << "start of terrain\n";
    
    terrain_main = Terrain(path, &materials);

}

World::World(Json::Value materials_json, Json::Value biome_data){
    init_materials(materials_json);

    std::vector<int> grass_grad_data;
    for (unsigned int i=0; i<materials_json["Dirt"]["Gradient"]["levels"].size(); i++){
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt());
    }

    std::cout << "start of terrain\n";

    terrain_main = Terrain(6, 6, 32, 128, 5, &materials, biome_data["Biome_1"], grass_grad_data, materials_json["Dirt"]["Gradient"]["midpoint"].asInt());
}

World::World(Json::Value biome_data, int tile_type){
    Json::Value materials_json;
    std::ifstream materials_file("../data/materials.json", std::ifstream::in);
    materials_file >> materials_json;

    init_materials(materials_json);

    terrain_main = Terrain(3, 3, 32, 128, 5, tile_type, &materials, biome_data);
}