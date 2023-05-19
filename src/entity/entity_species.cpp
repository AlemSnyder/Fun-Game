// Ths is the one that uses mesh, and loads qbs

#include "entity_species.hpp"

#include "../util/voxel.hpp"

entity::EntitySpecies::EntitySpecies(const Json::Value& entity_data){
    voxel_utility::VoxelObject model_data(entity_data["model"].asString());

    add_limb(model_data, entity_data["bones"]);
}

void
entity::EntitySpecies::add_limb(const voxel_utility::VoxelObject& model_data, const Json::Value& limb_data) {
    add_limb(glm::vec3(0), model_data, limb_data);
}

void
entity::EntitySpecies::add_limb(const glm::vec3 offset, const voxel_utility::VoxelObject& model_data, const Json::Value& limb_data) {

    // use layers to define separate 
    //auto mesh = generate_mesh(model_data[limb_data["limb_model_layer"].asString()]);

    auto mesh = generate_mesh(model_data, offset);

    gui::data_structures::Mesh data_mesh(mesh);

    limbs_.push_back(data_mesh);


    for (auto& child_limb : limb_data["children"]) {
        add_limb(model_data, child_limb);
    }
}