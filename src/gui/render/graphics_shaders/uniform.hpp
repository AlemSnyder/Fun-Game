#pragma once

#include <GL/glew.h>

#include <memory>
#include <string>
#include <vector>

namespace gui {

class OpenGLProgramExecuter;

namespace shader {

class Uniforms;

class Uniform {
    friend Uniforms;
    friend OpenGLProgramExecuter;

 protected:
    const std::string name_;

    GLint uniform_ID_;

 public:
    inline virtual const std::string&
    get_name() const {
        return name_;
    }

    // might want to add a get type ie vec3 etc

    virtual void bind() = 0;

    inline Uniform(std::string name) : name_(name), uniform_ID_(-1) {}
};

class Uniforms {
    friend OpenGLProgramExecuter;

 private:
    std::vector<std::shared_ptr<Uniform>> uniforms_;

 public:
    const std::vector<std::string> get_names() const;

    void bind() ;

    Uniforms(std::vector<std::shared_ptr<Uniform>> uniforms) : uniforms_(uniforms) {}

    auto
    begin() {
        return uniforms_.begin();
    }

    auto
    begin() const {
        return uniforms_.begin();
    }

    auto
    end() {
        return uniforms_.end();
    }

    auto
    end() const {
        return uniforms_.end();
    }
};

} // namespace shader

} // namespace gui
