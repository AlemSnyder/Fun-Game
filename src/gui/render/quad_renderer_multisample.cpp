#include "quad_renderer_multisample.hpp"

#include "../../util/files.hpp"
#include "../meshloader.hpp"
#include "../shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace render {

QuadRendererMultisample::QuadRendererMultisample(ShaderHandeler shader_handler) {
    // program
    programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTextureMS.frag"
    );
    texID = glGetUniformLocation(programID_, "texture");
    widthID = glGetUniformLocation(programID_, "width");
    heightID = glGetUniformLocation(programID_, "height");
    tex_samplesID = glGetUniformLocation(programID_, "tex_samples");

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

QuadRendererMultisample::~QuadRendererMultisample() {
    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteProgram(programID_);
}

void
QuadRendererMultisample::render(
    uint32_t width, uint32_t height, uint32_t samples, GLuint window_render_texture,
    GLuint frame_buffer
) const {
    // Render to the frame)buffer
    // if frame_buffer is 0 this is the screen
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Use our shader
    glUseProgram(programID_);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, window_render_texture);
    // Set our "renderedTexture" sampler to use Texture Unit 0
    glUniform1i(texID, 0);
    glUniform1ui(widthID, width);
    glUniform1ui(heightID, height);
    glUniform1ui(tex_samplesID, samples);

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
