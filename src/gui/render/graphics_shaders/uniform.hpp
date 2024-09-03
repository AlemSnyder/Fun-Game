// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file uniform.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Uniform Class
 *
 * @ingroup GUI  SHADER
 *
 */

#pragma once

#include <GL/glew.h>

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace gui {

namespace shader {

class UniformsVector;

class Uniform {
    friend UniformsVector;

 protected:
    const std::string name_;
    const std::string type_;

 public:
    inline virtual const std::string&
    get_name() const {
        return name_;
    }

    inline virtual const std::string&
    get_type() const {
        return type_;
    }

    virtual void bind(GLint uniform_ID) = 0;

    inline Uniform(std::string name, std::string type) : name_(name), type_(type) {}
};

class UniformsVector {
 private:
    std::vector<std::shared_ptr<Uniform>> uniforms_;

 public:
    std::set<std::pair<std::string, std::string>> get_names() const;

    UniformsVector(std::vector<std::shared_ptr<Uniform>> uniforms) :
        uniforms_(uniforms) {}

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
