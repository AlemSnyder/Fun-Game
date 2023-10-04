#include "quad_renderer_multisample.hpp"

#include "../../../util/files.hpp"
#include "../../shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace render {

QuadRendererMultisample::QuadRendererMultisample(ShaderHandler shader_handler) {
    // program
    program_id_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTextureMS.frag"
    );
    texID = glGetUniformLocation(program_id_, "texture_id");

    // The quad's FBO. Used only for visualizing the shadow map.
    static const std::vector<GLfloat> quad_vertices = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &quad_vertexbuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer_);
    glBufferData(
        GL_ARRAY_BUFFER, quad_vertices.size() * sizeof(quad_vertices[0]),
        quad_vertices.data(), GL_STATIC_DRAW
    );
    width_id_ = glGetUniformLocation(program_id_, "width");
    height_id_ = glGetUniformLocation(program_id_, "height");
    tex_samples_id_ = glGetUniformLocation(program_id_, "tex_samples");
}

} // namespace render

} // namespace gui
