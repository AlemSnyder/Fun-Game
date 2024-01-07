#pragma once

#include "opengl_program_status.hpp"

#include <GL/glew.h>

#include <string>

namespace gui {

namespace shader {

class Uniforms;

// forward declaration of program
class Program;

} // namespace shader

class OpenGLProgramExecuter {
 private:
    shader::Program& shader_program_;
    bool program_exists_ = true;

 public:
    OpenGLProgramExecuter(shader::Program& shader_program);

    virtual ~OpenGLProgramExecuter();

    // callback for when the program is updated
    virtual void reload_program() = 0;

    inline virtual void
    no_program() {
        program_exists_ = false;
    }

    [[nodiscard]] GLuint get_program_ID() const noexcept;

    inline void
    use_program() const {
        glUseProgram(get_program_ID());
    }

    // get program status

    [[nodiscard]] GLint inline get_uniform(std::string uniform_name) {
        return glGetUniformLocation(get_program_ID(), uniform_name.c_str());
    }


    void attach_uniforms(shader::Uniforms& uniforms);

    [[nodiscard]] shader::ProgramStatus get_program_status() const noexcept;
};

} // namespace gui