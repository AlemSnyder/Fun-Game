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
 * @file instanced_int.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines InstancedInt class
 *
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

#include "../../../entity/mesh.hpp"
#include "individual_int.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace data_structures {

/**
 * @brief Class to render instanced meshes
 *
 * @details Handles instanced meshes. Sends mesh data to GPU, and handles
 * binding, and deleting data on GPU.
 */
class InstancedInt : public IndividualInt {
 private:
    GLuint transforms_buffer_;
    uint32_t num_models_;

 public:
    inline InstancedInt(const InstancedInt& obj) = delete;
    // copy operator
    inline InstancedInt& operator=(const InstancedInt& obj) = delete;
    inline InstancedInt& operator=(InstancedInt&& other) = default;

    InstancedInt(
        const entity::Mesh& mesh, const std::vector<glm::ivec3>& model_transforms
    );

    inline ~InstancedInt() { glDeleteBuffers(1, &transforms_buffer_); }

    [[nodiscard]] inline GLuint
    get_model_transforms() const noexcept {
        return transforms_buffer_;
    }

    [[nodiscard]] inline uint32_t
    get_num_models() const noexcept {
        return num_models_;
    }
};

template <class T>
concept InstancedIntLike =
    std::is_base_of<gui::data_structures::InstancedInt, T>::value;

} // namespace data_structures

} // namespace gui
