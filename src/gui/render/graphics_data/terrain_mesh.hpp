#include "individual_int.hpp"

namespace gui {

namespace data_structures {

class TerrainMesh : public IndividualInt {
 public:
    TerrainMesh();
    TerrainMesh(const entity::Mesh& mesh, GLuint color_texture);

    void set_color_texture(GLuint color_texture) noexcept;

};

}

}
