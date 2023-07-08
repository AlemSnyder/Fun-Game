#include "static_mesh.hpp"

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../types.hpp"
#include "../meshloader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace data_structures {

Mesh::Mesh(
    const entity::Mesh& mesh, const std::vector<glm::ivec3>& model_transforms
) {
    // clear all buffers
    GLuint buffers[5] = {
        vertex_buffer_, color_buffer_, normal_buffer_, element_buffer_,
        transforms_buffer_};
    glDeleteBuffers(5, buffers);

    // if indices are none so if there is no vertices that would be sent to the graphics
    // card
    //     then there is no reason to create a buffer
    // create a bool do_render, set to false when
    num_vertices_ = mesh.get_indices().size();
    num_models_ = model_transforms.size();
    do_render_ = (num_vertices_ != 0 && num_models_ != 0);

    if (!do_render_) {
        return;
    }

    // A buffer for the vertex positions
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, mesh.get_indexed_vertices().size() * sizeof(glm::ivec3),
        mesh.get_indexed_vertices().data(), GL_STATIC_DRAW
    );

    // A buffer for the colors
    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, mesh.get_indexed_color_ids().size() * sizeof(uint16_t),
        mesh.get_indexed_color_ids().data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the normal vectors
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, mesh.get_indexed_normals().size() * sizeof(glm::i8vec3),
        mesh.get_indexed_normals().data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, mesh.get_indices().size() * sizeof(unsigned short),
        mesh.get_indices().data(), GL_STATIC_DRAW
    );

    // Generate a texture
    std::vector<ColorFloat> float_colors =
        color::convert_color_data(mesh.get_color_map());

    // LOG_DEBUG(logging::opengl_logger, "float_colors {}", float_colors);
    glGenTextures(1, &color_texture_);
    glBindTexture(GL_TEXTURE_1D, color_texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    glTexImage1D(
        GL_TEXTURE_1D, 0, GL_RGBA32F, float_colors.size(), 0, GL_RGBA, GL_FLOAT,
        float_colors.data()
    );
    glGenerateMipmap(GL_TEXTURE_1D);

    /// Generate a buffer for the transforms
    glGenBuffers(1, &transforms_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transforms_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, model_transforms.size() * sizeof(glm::ivec3),
        model_transforms.data(), GL_STATIC_DRAW
    );
}

} // namespace data_structures

} // namespace terrain
