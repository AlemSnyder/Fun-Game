#include "../types.hpp"

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
        size_t position_hash = static_cast<uint32_t>(
            vertex.position.x ^ vertex.position.y ^ vertex.position.z
        );

        /* clang-format off */
        uint8_t normal_hash = (vertex.normal.x << 2)
                            | (vertex.normal.y << 1)
                            | (vertex.normal.z);

        return position_hash << 32
             | vertex.mat_color_id << 16
             | normal_hash << 8
             | vertex.ambient_occlusion;
        /* clang-format on */
    }
};

