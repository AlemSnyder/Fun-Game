#pragma once

#include "opengl_program_status.hpp"

#include <GL/glew.h>

namespace gui {

namespace shader {

// forward decliration of program
class Program;

} // namespace shader

class OpenGLProgramExecuter {
 private:
    shader::Program& shader_program_;

 public:
    OpenGLProgramExecuter(shader::Program& shader_program);

    virtual ~OpenGLProgramExecuter();

    // callback for when the program is updated
    virtual void reload_program() = 0;

    [[nodiscard]] GLuint get_program_ID() const noexcept;
    // get program status

    [[nodiscard]] shader::ProgramStatus get_program_status() const noexcept;
};

} // namespace gui