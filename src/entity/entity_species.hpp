#include "../gui/data_structures/mesh.hpp"
#include "../util/voxel.hpp"
#include "entity.hpp"

#include <json/json.h>

#include <set>
#include <vector>

namespace entity {

// All the bees are controlled together
class EntitySpecies {
    // some set of general mesh
    std::vector<gui::data_structures::Mesh> limbs_;
    std::vector<Entity> controlled_entities;

 public:
    EntitySpecies(const Json::Value& entity_data);

    void add_limb(
        const voxel_utility::VoxelObject& model_data, const Json::Value& limb_data
    );
    void add_limb(
        const glm::vec3 offset, const voxel_utility::VoxelObject& model_data,
        const Json::Value& limb_data
    );

    void update_positions();
};

} // namespace entity
