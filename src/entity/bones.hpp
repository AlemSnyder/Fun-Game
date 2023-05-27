#include <json/json.h>

#include <glm/glm.hpp>

#include <vector>

// a bone like for animations

namespace entity {

struct Bone {
    size_t parent;               // static
    glm::mat4 relative_offset;   // static
    glm::mat4 relative_position; // changes per frame
    glm::mat4 postion;           // changes every frame
    //
};

class Bones {
 private:
    std::vector<Bone> bones_;

 public:
    Bones(const Json::Value& bone_structure);
    //    ~Bones();
    void update(std::vector<glm::vec3> angles);
    void add_bone(int parent, const Json::Value& json_bone);

    [[nodiscard]] inline size_t
    size() {
        return bones_.size();
    }

    glm::mat4 get_position(size_t bone_index);
};

} // namespace entity
