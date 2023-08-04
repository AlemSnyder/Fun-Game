#include "quad_renderer_multisample.hpp"

#include "../../../util/files.hpp"
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

} // namespace render

} // namespace gui
