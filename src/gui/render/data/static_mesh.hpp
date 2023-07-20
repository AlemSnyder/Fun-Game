#include "instanced_int.hpp"

#pragma once

namespace gui {

namespace data_structures {

class StaticMesh : public InstancedInt {
 public:
    StaticMesh(const entity::Mesh& mesh,
    const std::vector<glm::ivec3>& model_transforms);

 protected:
    void generate_color_texture(const entity::Mesh& mesh);

};

}

}
