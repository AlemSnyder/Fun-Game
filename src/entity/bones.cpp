#include "bones.hpp"

#include <json/json.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

entity::Bones::Bones(const Json::Value& bone_structure) {
    // This will be -1 until nino tels me to change
    // this to a size_t
    add_bone(-1, bone_structure);
}

void
entity::Bones::add_bone(int parent, const Json::Value& json_bone) {

    auto& json_offset = json_bone["offset"];
    auto vector_offset = glm::vec3(
        json_offset[0].asFloat(), json_offset[1].asFloat(), json_offset[2].asFloat()
    );
    glm::mat4 matrix_offset = glm::translate(glm::mat4(), vector_offset);
    Bone bone{parent, matrix_offset, glm::mat4(), glm::mat4()};

    bones_.push_back(bone);

    int parent_bone_index = bones_.size() - 1;

    for (auto& child_bone : json_bone["children"]) {
        add_bone(parent_bone_index, child_bone);
    }
}

void
entity::Bones::update(std::vector<glm::vec3> angles) {
    glm::vec3 i(1, 0, 0);
    glm::vec3 j(0, 1, 0);
    glm::vec3 k(0, 0, 1);

    if (bones_.size() != angles.size()) {
        // warning
        return;
    }

    for (size_t n = 0; n < angles.size(); n++) {
        auto theta = angles[n].x;
        auto phi = angles[n].y;
        auto rho = angles[n].z;

        glm::mat4 rotation = glm::rotate(
            glm::rotate(glm::rotate(glm::mat4(), theta, i), phi, j), rho, k
        );

        bones_[n].relative_position = bones_[n].relative_offset * rotation;
    }

    // base position of main bone
    bones_[0].postion = glm::mat4(1.0f); // replace with given

    for (size_t n = 1; n < angles.size(); n++) {
        bones_[n].postion =
            bones_[bones_[n].parent].postion * bones_[n].relative_position;
    }
}
