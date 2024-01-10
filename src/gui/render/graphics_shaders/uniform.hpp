#pragma once

#include <GL/glew.h>

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <utility>

namespace gui {

class OpenGLProgramExecuter;

namespace shader {

class Uniforms;

class Uniform {
    friend Uniforms;
    friend OpenGLProgramExecuter;

 protected:
    const std::string name_;
    const std::string type_;

    GLint uniform_ID_;

 public:
    inline virtual const std::string&
    get_name() const {
        return name_;
    }

    inline virtual const std::string&
    get_type() const {
        return type_;
    }

    virtual void bind() = 0;

    inline Uniform(std::string name, std::string type) : name_(name), type_(type), uniform_ID_(-1) {}
};

class Uniforms {
    friend OpenGLProgramExecuter;

 private:
    std::vector<std::shared_ptr<Uniform>> uniforms_;

 public:
    std::set<std::pair<std::string, std::string>> get_names() const;

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
