#include "quad_renderer.hpp"

#include "../../util/files.hpp"
#include "../handler.hpp"
#include "../meshloader.hpp"
#include "../shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {
namespace render {

QuadRenderer::QuadRenderer() {
    // program
    programID_ = load_shaders(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTexture.frag"
    );
    texID = glGetUniformLocation(programID_, "texture");

    // The quad's FBO. Used only for visualizing the shadow map.
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data,
        GL_STATIC_DRAW
    );
}

QuadRenderer::~QuadRenderer() {
    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteProgram(programID_);
}

void
QuadRenderer::render(
    screen_size_t width, screen_size_t height, GLuint window_render_texture,
    GLuint frame_buffer
) const {
    // Render to the screen
    FrameBufferHandler::getInstance().bind_fbo(frame_buffer);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT);
    // Use our shader
    glUseProgram(programID_);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window_render_texture);
    // Set our "renderedTexture" sampler to use Texture Unit 0
    glUniform1i(texID, 0);

    // first attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(
        0,        // attribute 0. No particular reason for 0,
                  // but must match the layout in the shader.
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0  // array buffer offset
    );

    // Draw the triangle !
    // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting
    // at 0 -> 2 triangles
    glDisableVertexAttribArray(0);
}

} // namespace render

} // namespace gui
