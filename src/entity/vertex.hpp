#include "../types.hpp"
#include "../util/hash_combine.hpp"

#include <glm/glm.hpp>

#include <functional>

namespace entity {

struct Vertex {
    VoxelOffset position;
    glm::i8vec3 normal;
    MatColorId mat_color_id;
    uint8_t ambient_occlusion;

    bool
    operator==(const Vertex& other) const {
        return (
            other.position == position && other.normal == normal
            && other.mat_color_id == mat_color_id
            && other.ambient_occlusion == ambient_occlusion
        );
    }
};

} // namespace entity

template <>
struct std::hash<entity::Vertex> {
    size_t
    operator()(const entity::Vertex& vertex) const {
        size_t result = 0;

        // Position
        utils::hash_combine(result, vertex.position.x);
        utils::hash_combine(result, vertex.position.y);
        utils::hash_combine(result, vertex.position.z);

        // Normal
        utils::hash_combine(result, vertex.normal.x << 2);
        utils::hash_combine(result, vertex.normal.y << 1);
        utils::hash_combine(result, vertex.normal.z);

        // Color ID and ambient occlusion
        utils::hash_combine(result, vertex.mat_color_id);
        utils::hash_combine(result, vertex.ambient_occlusion);

        return result;
    }
};
