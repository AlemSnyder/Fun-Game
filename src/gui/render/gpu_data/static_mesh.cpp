#include "static_mesh.hpp"

namespace gui {

namespace gpu_data {

void
StaticMesh::generate_color_texture(const world::entity::Mesh& mesh) {
    // Generate a texture
    std::vector<ColorFloat> float_colors =
        color::convert_color_data(mesh.get_color_map());

    // create one texture and save the id to color_texture_
    glGenTextures(1, &color_texture_);
    // bind to color_texture_
    glBindTexture(GL_TEXTURE_1D, color_texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // set texture warp. values outside of texture bounds are clamped to
    // edge of texture
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // no minimapping so this one don't matter
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // set magnification filter to linear. Interpolate between pixels in mat
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    glTexImage1D(
        GL_TEXTURE_1D, 0, GL_RGBA32F, float_colors.size(), 0, GL_RGBA, GL_FLOAT,
        float_colors.data()
    );
    glGenerateMipmap(GL_TEXTURE_1D);
}

// TODO fix order
void
StaticMesh::update_position(uint offset, std::vector<glm::ivec4> data) {
    transforms_array_.update(data, offset);
}

} // namespace gpu_data

} // namespace gui
