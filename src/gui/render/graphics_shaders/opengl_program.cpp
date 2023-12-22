#include "opengl_program.hpp"

#include "shader.hpp"

namespace gui {

OpenGLProgramExecuter::OpenGLProgramExecuter(shader::Program& shader_program) :
    shader_program_(shader_program) {
    shader_program_.add_executor(this);
}

OpenGLProgramExecuter::~OpenGLProgramExecuter() {
    shader_program_.remove_executor(this);
}

GLuint
gui::OpenGLProgramExecuter::get_program_ID() const noexcept {
    return shader_program_.get_program_ID();
}

gui::shader::ProgramStatus
gui::OpenGLProgramExecuter::get_program_status() const noexcept {
    return shader_program_.get_status();
}

} // namespace gui
