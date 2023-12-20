#include "quad_renderer_multisample.hpp"

#include "../../../util/files.hpp"
#include "../../shader.hpp"

#include <GL/glew.h>

#include <memory>

namespace gui {

namespace render {

QuadRendererMultisample::QuadRendererMultisample(shader::Program& program) :
    QuadRenderer(program) {
    reload_program();

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
}

void
QuadRendererMultisample::reload_program() {
    GLuint program_id = get_program_ID();
    texID = glGetUniformLocation(program_id, "texture_id");
    width_id_ = glGetUniformLocation(program_id, "width");
    height_id_ = glGetUniformLocation(program_id, "height");
    tex_samples_id_ = glGetUniformLocation(program_id, "tex_samples");
}

} // namespace render

} // namespace gui
