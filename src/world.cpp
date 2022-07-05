#include "world.hpp"
#include "terrain.hpp"
#include <fstream>
#include <stdint.h>

void World::save(){
    terrain_main.qb_save(path);
}

const Material* World::get_material(int material_id) const {
    return &materials.at(material_id);
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

void World::get_mesh_greedy(std::vector<unsigned short> &indices,
					std::vector<glm::vec3> &indexed_vertices,
					std::vector<glm::vec3> &indexed_uvs,
					std::vector<glm::vec3> &indexed_normals) const {
    const int dims[3] = {terrain_main.get_X_MAX(), terrain_main.get_Y_MAX(), terrain_main.get_Z_MAX()};
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // printf("axis: %ld\n", axis);

        const std::size_t u = (axis + 1) % 3;
        const std::size_t v = (axis + 2) % 3;

        // printf("u: %ld, v: %ld\n", u, v);

        int x[3] = {0};
        int q[3] = {0};
        std::vector<int> mask(dims[u] * dims[v]);
        // printf("dims[u]: %d, dims[v]: %d\n", dims[u], dims[v]);

        // printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
        // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

        // Compute mask
        q[axis] = 1;
        for (x[axis] = -1; x[axis] < dims[axis];) {
            // printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
            // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

            std::size_t counter = 0;
            for (x[v] = 0; x[v] < dims[v]; ++x[v])
                for (x[u] = 0; x[u] < dims[u]; ++x[u], ++counter) {
                    const int a = 0 <= x[axis] ? terrain_main.get_tile(x[0], x[1], x[2])->get_mat_color_id() : 0;
                    const int b = x[axis] < dims[axis] - 1
                                      ? terrain_main.get_tile(x[0] + q[0], x[1] + q[1], x[2] + q[2])->get_mat_color_id()
                                      : 0;
                    const bool ba = static_cast<bool>(a);
                    if (ba == static_cast<bool>(b))
                        mask[counter] = 0;
                    else if (ba)
                        mask[counter] = a;
                    else
                        mask[counter] = -b;
                }

            ++x[axis];

            // Generate mesh for mask using lexicographic ordering
            int width = 0, height = 0;

            counter = 0;
            for (int j = 0; j < dims[v]; ++j)
            for (int i = 0; i < dims[u];) {
                int c = mask[counter];
                if (c) {
                    // Compute width
                    for (width = 1;
                            c == mask[counter + width] && i + width < dims[u];
                            ++width) {
                    }

                    // Compute height
                    bool done = false;
                    for (height = 1; j + height < dims[v]; ++height) {
                        for (int k = 0; k < width; ++k)
                            if (c != mask[counter + k + height * dims[u]]) {
                                done = true;
                                break;
                            }

                        if (done) break;
                    }

                    // Add quad
                    x[u] = i;
                    x[v] = j;

                    int du[3] = {0}, dv[3] = {0};

                    if (c > 0) {
                        dv[v] = height;
                        du[u] = width;
                    } else {
                        c = -c;
                        du[v] = height;
                        dv[u] = width;
                    }

                    const std::size_t vertexSize = indexed_vertices.size();
                    indexed_vertices.push_back(glm::vec3(x[0], x[1], x[2]));
                    indexed_vertices.push_back(glm::vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]));
                    indexed_vertices.push_back(glm::vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]));
                    indexed_vertices.push_back(glm::vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]));

                    
                    uint8_t color_id = uint16_t(c) & 0xFF;
                    uint8_t material_id = (uint16_t(c) >> 8) & 0xFF;
                    uint32_t int_color = get_material(material_id)->color[color_id].second;
                    glm::vec3 color(float( ( int_color >> 24 ) & 0xFF ) / 255.0,float( ( int_color >> 16 ) & 0xFF ) / 255.0, float( ( int_color >> 8 ) & 0xFF ) / 255.0);
                    indexed_uvs.push_back(color);
                    indexed_uvs.push_back(color);
                    indexed_uvs.push_back(color);
                    indexed_uvs.push_back(color);

                    glm::vec3 normal = glm::normalize(glm::cross(indexed_vertices[vertexSize] - indexed_vertices[vertexSize+1],
                                                                indexed_vertices[vertexSize] - indexed_vertices[vertexSize+2]));
                    indexed_normals.push_back(normal);
                    indexed_normals.push_back(normal);
                    indexed_normals.push_back(normal);
                    indexed_normals.push_back(normal);

                    indices.push_back(vertexSize);
                    indices.push_back(vertexSize+1);
                    indices.push_back(vertexSize+2);
                    indices.push_back(vertexSize+2);
                    indices.push_back(vertexSize+3);
                    indices.push_back(vertexSize);

                    for (int b = 0; b < width; ++b)
                        for (int a = 0; a < height; ++a)
                            mask[counter + b + a * dims[u]] = 0;

                    // Increment counters
                    i += width;
                    counter += width;
                } else {
                    ++i;
                    ++counter;
                }
            }
        }
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
