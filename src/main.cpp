#include <iostream>
//#include <vector>
#include "terrain.hpp"
#include <iostream>
#include <string>
#include "world.hpp"
#include <fstream>

int test1(){

    //const char * home_path = "C:/Users/haile/Documents/School/Comp Sci but C/gcc/terrain_generation";

    Json::Value materials_json;
    std::ifstream materials_file("../data/materials.json", std::ifstream::in);
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;

    const char * path = "../SavedTerrain/save.qb";

    World world(materials_json, biome_data);

    world.terrain_main.qb_save_debug(path, world.get_materials());

    return 0;
}

int test2(){

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;


    Terrain::generate_macro_map(64,64, biome_data["Biome_1"]["Terrain_Data"]);

    return 1;
}

int savetest(){
    const char * path = "../SavedTerrain/save.qb";
    const char * save_path = "../SavedTerrain/save_test.qb";

    World world(path);

    world.terrain_main.qb_save(save_path);

    return 0;
}

void save_terrain(Json::Value biome_data, std::string biome_name){
    World world;

    std::cout << "Saving " << biome_data["Tile_Data"].size() << " tile types" << std::endl;

    //std::cout << biome_data["Tile_Data"] << std::endl;

    for (unsigned int i = 0; i < biome_data["Tile_Data"].size(); i++){
        world.terrain_main.init(3,3, 32, 128, 5, (int) i, world.get_materials(), biome_data);
        std::string path("../SavedTerrain/");
        path += biome_name;
        path += "/biome_";
        path += std::to_string(i);
        path += ".qb";
        world.terrain_main.qb_save(path.c_str());
    }
    //Json::Value biome_data;
    //std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    //biome_file >> biome_data;
}

void save_all_terrain(Json::Value biome_data){
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end(); biome_type++){
        save_terrain((*biome_type), biome_type.key().asCString());
    }
}

int pathfindertest(){
    const char * path = "../SavedTerrain/pathfinder_input.qb";
    const char * save_path = "../SavedTerrain/pathfinder_output.qb";

    World world(path);

    std::pair<Tile *, Tile *> start_end = world.terrain_main.get_start_end_test();

    std::vector<Tile*> tile_path = world.terrain_main.get_path_Astar(start_end.first, start_end.second);

    if (tile_path.size() == 0){
        std::cout << "no path" << std::endl;
        world.terrain_main.qb_save_debug(path, world.get_materials());
    }

    for (auto it = tile_path.begin(); it != tile_path.end(); ++it){
        (*it)->set_material(&world.get_materials()->at(6),1);
    }

    world.terrain_main.qb_save(save_path);

    return 0;
}

int main( int argc, char** argv ){
    if (argc == 1){
        return pathfindertest();
    } else if (std::string(argv[1]) == "--TerrainTypes"){
        Json::Value biome_data;
        std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
        biome_file >> biome_data;
        if (argc == 3){
            save_terrain(biome_data[argv[2]],argv[2]);
        }
        else{
            save_all_terrain(biome_data);
        }
    } else if (std::string(argv[1]) == "--MacroMap"){
        return test2();
    }
    else if (std::string(argv[1]) == "--SaveTest"){
        return savetest();
    }
    else if (std::string(argv[1]) == "--pathfinder"){
        return pathfindertest();
    }
}
