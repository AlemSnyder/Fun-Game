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
 * @file floating_instanced_i_mesh.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines FloatingInstancedIMeshGPU class
 *
 * @ingroup GUI  GPU_DATA
 *
 */

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
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
 * @brief Class to render integer meshes with arbitrary position
 *
 * @details Handles meshes, and the position. Sends mesh data to GPU, and handles
 * binding, and deleting data on GPU.
 */
class FloatingInstancedIMeshGPU :
    public virtual IMeshGPU,
    public virtual GPUDataElementsInstanced {
 protected:
    VertexBufferObject<glm::mat4> transforms_array_;

 public:
    inline FloatingInstancedIMeshGPU(const FloatingInstancedIMeshGPU& obj) = delete;
    inline FloatingInstancedIMeshGPU(FloatingInstancedIMeshGPU&& other) = default;
    // copy operator
    inline FloatingInstancedIMeshGPU& operator=(const FloatingInstancedIMeshGPU& obj
    ) = delete;
    inline FloatingInstancedIMeshGPU& operator=(FloatingInstancedIMeshGPU&& other
    ) = default;

    inline FloatingInstancedIMeshGPU(const world::entity::Mesh& mesh) :
        FloatingInstancedIMeshGPU(mesh, {}) {}

    FloatingInstancedIMeshGPU(
        const world::entity::Mesh& mesh, const std::vector<glm::mat4>& model_transforms
    );

    inline virtual ~FloatingInstancedIMeshGPU() { }

    virtual void attach_all() override;

    void update_transforms_array(std::vector<glm::mat4> data, uint offset);

    [[nodiscard]] inline const VertexBufferObject<glm::mat4>&
    get_model_transforms() const noexcept {
        return transforms_array_;
    }

    [[nodiscard]] inline uint32_t virtual get_num_models() const noexcept override {
        return transforms_array_.size();
    }
};

} // namespace gpu_data

} // namespace gui
