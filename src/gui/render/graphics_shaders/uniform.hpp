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
#include "uniform_exicuter.hpp"

#include <GL/glew.h>

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace gui {

namespace shader {

/**
 * @brief Representation of a uniform
 *
 * @details Use virtual inheritance to extend this class.
 * overwrite the bind method to send uniform data to the given
 * uniform ID.
 */
class Uniform {
 protected:
    const std::string name_;           // name used in shader program
    const gpu_data::GPUDataType type_; // glsl type as stirng

    const GLint uid_;

    std::shared_ptr<UniformExecuter> call_function_;

 public:
    /**
     * @brief Get the name of the uniform
     *
     * @return const std::stirng& name used in shader program
     */
    inline const std::string&
    get_name() const noexcept {
        return name_;
    }

    /**
     * @brief Get the glsl type as a string
     *
     * @return const GPUDataType type
     */
    inline const gpu_data::GPUDataType&
    get_type() const noexcept {
        return type_;
    }

    /**
     * @brief Get the uniform id
     *
     * @return GLuint uid
     */
    [[nodiscard]] inline GLuint
    get_uid() const noexcept {
        return uid_;
    }

    /**
     * @brief Virtual method to override
     *
     * @details Use glUniform{__} to send data to the given uniform ID
     *
     * @param Glint uniform_ID uniform ID to send data to.
     */
    inline void
    bind() const {
        if (call_function_) {
            call_function_->bind(uid_);
        }
    }

    /**
     * @brief Set the executor that will set the uniform
     */
    inline void
    set_call_function(std::shared_ptr<UniformExecuter> call_function) {
        call_function_ = call_function;
    }

    /**
     * @brief Construct a new Uniform object
     *
     * @param std::string name name of uniform used in shader program
     * @param gpu_data::GPUDataType type
     */
    inline Uniform(std::string name, gpu_data::GPUDataType type, GLuint uid) :
        name_(name), type_(type), uid_(uid) {}
};

} // namespace shader

} // namespace gui
