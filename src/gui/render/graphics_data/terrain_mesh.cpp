#include "terrain_mesh.hpp"

namespace gui {

namespace data_structures {

TerrainMesh::TerrainMesh(){
    // required to make a shared pointer
}

TerrainMesh::TerrainMesh(const entity::Mesh& mesh, GLuint color_texture_id) :
    IndividualInt(mesh) {
    set_color_texture(color_texture_id);
}

void
TerrainMesh::set_color_texture(GLuint color_texture_id) noexcept {
    color_texture_ = color_texture_id;
}

} // namespace data_structures

} // namespace gui
