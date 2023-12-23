#include "opengl_program.hpp"

#include "shader.hpp"

namespace gui {

OpenGLProgramExecuter::OpenGLProgramExecuter(shader::Program& shader_program) :
    shader_program_(shader_program) {
    shader_program_.add_executor(this);
}

OpenGLProgramExecuter::~OpenGLProgramExecuter() {
    if (program_exists_)
        shader_program_.remove_executor(this);
}

GLuint
gui::OpenGLProgramExecuter::get_program_ID() const noexcept {
    if (program_exists_)
        return shader_program_.get_program_ID();
    return 0;
}

gui::shader::ProgramStatus
gui::OpenGLProgramExecuter::get_program_status() const noexcept {
    if (program_exists_)
        return shader_program_.get_status();
    return shader::ProgramStatus::EMPTY;
}

} // namespace gui
