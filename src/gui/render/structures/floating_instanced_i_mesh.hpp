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
 * @ingroup GUI  array_buffer
 *
 */

#include "gui/render/gpu_data/array_buffer.hpp"
#include "gui/render/gpu_data/data_types.hpp"
#include "i_mesh.hpp"
#include "world/entity/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace gpu_data {

/**
 * @brief Class to render instanced meshes
 *
 * @details Handles instanced meshes. Sends mesh data to GPU, and handles
 * binding, and deleting data on GPU.
 */
class FloatingInstancedIMeshGPU :
    public virtual IMeshGPU,
    public virtual GPUDataElementsInstanced {
 protected:
    ArrayBuffer<glm::mat4> transforms_array_;
    uint32_t num_models_;

 public:
    inline FloatingInstancedIMeshGPU(const FloatingInstancedIMeshGPU& obj) = delete;
    inline FloatingInstancedIMeshGPU(FloatingInstancedIMeshGPU&& other) = default;
    // copy operator
    inline FloatingInstancedIMeshGPU& operator=(const FloatingInstancedIMeshGPU& obj
    ) = delete;
    inline FloatingInstancedIMeshGPU& operator=(FloatingInstancedIMeshGPU&& other
    ) = default;

    FloatingInstancedIMeshGPU(
        const world::entity::Mesh& mesh, const std::vector<glm::mat4>& model_transforms
    );

    void update_transforms_array(std::vector<glm::mat4> data, uint offset);

    inline void virtual bind() const override {
        IMeshGPU::bind();
        transforms_array_.bind(6);
        // will bind to 3, 4, 5, 6 because mat4
        // here is a random GitHub link to fallow
        // https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/4.advanced_opengl/10.3.asteroids_instanced/asteroids_instanced.cpp#L136
        // One can only send vec4 to through a buffer. To send a mat4 multiple buffers
        // are used.
    }

    inline void virtual release() const override {
        IMeshGPU::release();
        glDisableVertexAttribArray(6);
        glDisableVertexAttribArray(7);
        glDisableVertexAttribArray(8);
        glDisableVertexAttribArray(9);
    }

    [[nodiscard]] inline const ArrayBuffer<glm::mat4>&
    get_model_transforms() const noexcept {
        return transforms_array_;
    }

    [[nodiscard]] inline uint32_t virtual get_num_models() const noexcept override {
        return num_models_;
    }
};

} // namespace gpu_data

} // namespace gui
