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
    program_id_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTextureMS.frag"
    );
    width_id_ = glGetUniformLocation(program_id_, "width");
    height_id_ = glGetUniformLocation(program_id_, "height");
    tex_samples_id_ = glGetUniformLocation(program_id_, "tex_samples");

}

void
QuadRendererMultisample::render(
    screen_size_t width, screen_size_t height, uint32_t samples,
    GLuint window_render_texture, GLuint frame_buffer
) const {

    QuadRenderer::setup(width, height, window_render_texture, frame_buffer);
    glUniform1ui(width_id_, width);
    glUniform1ui(height_id_, height);
    glUniform1ui(tex_samples_id_, samples);
    QuadRenderer::draw();
}

} // namespace render

} // namespace gui
