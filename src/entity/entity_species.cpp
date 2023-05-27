// Ths is the one that uses mesh, and loads qbs

#include "entity_species.hpp"

#include "../util/voxel.hpp"

namespace entity {

EntitySpecies::EntitySpecies(const Json::Value& entity_data) {
    voxel_utility::VoxelObject model_data(entity_data["model"].asString());

    add_limb(model_data, entity_data["bones"]);
}

void
EntitySpecies::add_limb(
    const voxel_utility::VoxelObject& model_data, const Json::Value& limb_data
) {
    add_limb(glm::vec3(0), model_data, limb_data);
}

void
EntitySpecies::add_limb(
    const glm::vec3 offset, const voxel_utility::VoxelObject& model_data,
    const Json::Value& limb_data
) {
    // use layers to define separate
    // TODO make qb data have this indexing
    // auto mesh = generate_mesh(model_data[limb_data["limb_model_layer"].asString()]);

    auto mesh = generate_mesh(model_data, offset);

    gui::data_structures::Mesh data_mesh(mesh);

    limbs_.push_back(data_mesh);

    for (auto& child_limb : limb_data["children"]) {
        add_limb(model_data, child_limb);
    }
}

void
EntitySpecies::update_positions() {
    // a vector of transforms for each bone
    std::vector<std::vector<glm::mat4>> transforms;
    for (size_t i = 0; i < limbs_.size(); i++) {
        transforms.push_back(std::vector<glm::mat4>());
    }
    for (auto& e : controlled_entities) {
        e.update_bones();
        for (size_t bone_index = 0; bone_index < e.num_bones(); bone_index++) {
            transforms[bone_index].push_back(e.bone_transform(bone_index));
        }
    }

    for (auto& limb : limbs_) {
        limb.update_transforms(transforms[limb.get_bone_index()]);
    }
}

} // namespace entity
