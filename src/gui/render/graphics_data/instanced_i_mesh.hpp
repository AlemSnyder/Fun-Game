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
 * @file instanced_i_mesh.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines InstancedInt class
 *
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

#include "../../../entity/mesh.hpp"
#include "array_buffer.hpp"
#include "gpu_data.hpp"
#include "non_instanced_i_mesh.hpp"

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
class InstancedIMeshGPU : public virtual NonInstancedIMeshGPU {
 private:
    ArrayBuffer transforms_array_;
    uint32_t num_models_;

 public:
    inline InstancedIMeshGPU(const InstancedIMeshGPU& obj) = delete;
    inline InstancedIMeshGPU(InstancedIMeshGPU&& other) = default;
    // copy operator
    inline InstancedIMeshGPU& operator=(const InstancedIMeshGPU& obj) = delete;
    inline InstancedIMeshGPU& operator=(InstancedIMeshGPU&& other) = default;

    InstancedIMeshGPU(
        const entity::Mesh& mesh, const std::vector<glm::ivec3>& model_transforms
    );

    inline void virtual bind() const override {
        NonInstancedIMeshGPU::bind();
        transforms_array_.bind(3, 3);
    }

    inline void virtual release() const override {
        NonInstancedIMeshGPU::release();
        glDisableVertexAttribArray(3);
    }

    [[nodiscard]] inline const ArrayBuffer&
    get_model_transforms() const noexcept {
        return transforms_array_;
    }

    [[nodiscard]] inline uint32_t virtual get_num_models() const noexcept {
        return num_models_;
    }
};

} // namespace data_structures

} // namespace gui
