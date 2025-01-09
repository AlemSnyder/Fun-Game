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

#include "gui/render/gl_enums.hpp"

#include <GL/glew.h>

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace gui {

namespace shader {

class UniformsVector;

/**
 * @brief Representation of a uniform
 *
 * @details Use virtual inheritance to extend this class.
 * overwrite the bind method to send uniform data to the given
 * uniform ID.
 */
class Uniform {
    friend UniformsVector;

 protected:
    const std::string name_; // name used in shader program
    const gpu_data::GPUDataType type_; // glsl type as stirng

 public:
    /**
     * @brief Get the name of the uniform
     *
     * @return const std::stirng& name used in shader program
     */
    inline virtual const std::string&
    get_name() const {
        return name_;
    }

    /**
     * @brief Get the glsl type as a string
     *
     * @return const GPUDataType type
     */
    inline virtual const gpu_data::GPUDataType&
    get_type() const {
        return type_;
    }

    /**
     * @brief Virtual method to override
     *
     * @details Use glUniform{__} to send data to the given uniform ID
     *
     * @param Glint uniform_ID uniform ID to send data to.
     */
    virtual void bind(GLint uniform_ID) = 0;

    /**
     * @brief Construct a new Uniform object
     *
     * @param std::string name name of uniform used in shader program
     * @param gpu_data::GPUDataType type
     */
    inline Uniform(std::string name, gpu_data::GPUDataType type) : name_(name), type_(type) {}
};

/**
 * @brief The set of uniforms used in any particular program
 */
class UniformsVector {
 private:
    // why are these a vector? I guess I don't want to bother with a hash,
    // and it's not like we need to insert many things into this
    std::vector<std::shared_ptr<Uniform>> uniforms_;

 public:
    /**
     * @brief Get a set of all names and their corresponding type of uniforms
     * in this object.
     *
     * @return std::set<std::pair<std::string, gpu_data::GPUDataType>> set of [names, type]
     */
    std::set<std::pair<std::string, gpu_data::GPUDataType>> get_names() const;

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
