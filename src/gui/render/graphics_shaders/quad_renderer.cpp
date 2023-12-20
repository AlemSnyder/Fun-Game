#include "quad_renderer.hpp"

#include "../../handler.hpp"

namespace gui {

namespace render {

QuadRenderer::QuadRenderer(shader::Program& shader_program) :
    OpenGLProgramExecuter(shader_program) {
    // program
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

QuadRenderer::~QuadRenderer() {
    glDeleteBuffers(1, &quad_vertexbuffer_);
    
}

void
QuadRenderer::setup(
    screen_size_t width, screen_size_t height, GLuint window_render_texture,
    GLuint frame_buffer
) const {
    // Render to the screen
    FrameBufferHandler::instance().bind_fbo(frame_buffer);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT);
    // Use our shader
    glUseProgram(get_program_ID());

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window_render_texture);
    // Set our "renderedTexture" sampler to use Texture Unit 0
    glUniform1i(texID, 0);
}

void
QuadRenderer::draw() const {
    // first attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer_);
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

void
QuadRenderer::render(
    screen_size_t width, screen_size_t height, GLuint window_render_texture,
    GLuint frame_buffer
) const {
    setup(width, height, window_render_texture, frame_buffer);
    draw();
}

} // namespace render

} // namespace gui
