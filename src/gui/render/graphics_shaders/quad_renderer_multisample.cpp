#include "quad_renderer_multisample.hpp"

#include "../../../util/files.hpp"
#include "../../meshloader.hpp"
#include "../../shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace render {

QuadRendererMultisample::QuadRendererMultisample(ShaderHandler shader_handler) :
    QuadRenderer(shader_handler) {
    // program
    programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTextureMS.frag"
    );
    widthID = glGetUniformLocation(programID_, "width");
    heightID = glGetUniformLocation(programID_, "height");
    tex_samplesID = glGetUniformLocation(programID_, "tex_samples");

}

QuadRendererMultisample::~QuadRendererMultisample() {
    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteProgram(programID_);
}

int
QuadRendererMultisample::render(
    screen_size_t width, screen_size_t height, uint32_t samples,
    GLuint window_render_texture, GLuint frame_buffer
) const {

    QuadRenderer::setup(width, height, window_render_texture, frame_buffer);
    glUniform1ui(widthID, width);
    glUniform1ui(heightID, height);
    glUniform1ui(tex_samplesID, samples);
    QuadRenderer::draw();
    return 0;
}

} // namespace render

} // namespace gui
