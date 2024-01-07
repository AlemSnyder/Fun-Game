#include "opengl_program.hpp"

#include "program_handler.hpp"
#include "uniform.hpp"

namespace gui {

OpenGLProgramExecuter::OpenGLProgramExecuter(shader::Program& shader_program) :
    shader_program_(shader_program) {
    shader_program_.add_executor(this);
}

OpenGLProgramExecuter::~OpenGLProgramExecuter() {
    if (program_exists_) [[likely]]
        shader_program_.remove_executor(this);
}

GLuint
OpenGLProgramExecuter::get_program_ID() const noexcept {
    if (program_exists_) [[likely]]
        return shader_program_.get_program_ID();
    return 0;
}

shader::ProgramStatus
OpenGLProgramExecuter::get_program_status() const noexcept {
    if (program_exists_) [[likely]]
        return shader_program_.get_status();
    return shader::ProgramStatus::EMPTY;
}

void
OpenGLProgramExecuter::attach_uniforms(shader::Uniforms& uniforms) {
    for (auto& uniform : uniforms) {
        uniform->uniform_ID_ = get_uniform(uniform->get_name());
    }
}

} // namespace gui
