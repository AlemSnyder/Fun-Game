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
    main_PID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "overlay" / "SimpleTextureMS.frag"
    );
    texture_UID_ = glGetUniformLocation(main_PID_, "texture_id");

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
    width_UID_ = glGetUniformLocation(main_PID_, "width");
    height_UID_ = glGetUniformLocation(main_PID_, "height");
    num_samples_UID_ = glGetUniformLocation(main_PID_, "tex_samples");
}

} // namespace render

} // namespace gui
