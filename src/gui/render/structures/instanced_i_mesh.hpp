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
 * @brief Class to render instanced meshes
 *
 * @details Handles instanced meshes. Sends mesh data to GPU, and handles
 * binding, and deleting data on GPU. The point of this class is that the
 * transforms array should be updated often.
 *
 * @warning Maybe want to remove inheritance because we want to limit the data
 * size. NonInstancedIMeshGPU uses ints to save position, but models should only
 * need 5-6 bits.
 */
class InstancedIMeshGPU :
    public virtual IMeshGPU,
    public virtual GPUDataElementsInstanced {
 protected:
    VertexBufferObject<glm::ivec4> transforms_array_;
    uint32_t num_models_;

 public:
    inline InstancedIMeshGPU(const InstancedIMeshGPU& obj) = delete;
    inline InstancedIMeshGPU(InstancedIMeshGPU&& other) = default;
    // copy operator
    inline InstancedIMeshGPU& operator=(const InstancedIMeshGPU& obj) = delete;
    inline InstancedIMeshGPU& operator=(InstancedIMeshGPU&& other) = default;

    /**
     * @brief Construct a new InstancedIMeshGPU instance
     *
     * @param const Mesh& mesh
     * @param const std::vector<glm::ivec4>& model_transforms
     */
    InstancedIMeshGPU(
        const world::entity::Mesh& mesh, const std::vector<glm::ivec4>& model_transforms
    );

    inline virtual ~InstancedIMeshGPU() { }

    virtual void attach_all() override;

    /**
     * @brief Write given data to the transforms array starting at the given offset.
     *
     * @param std::vector<glm::ivec4> data data to be written
     * @param uint offset start index to write data
     */
    void update_transforms_array(std::vector<glm::ivec4> data, uint offset);

    /**
     * @brief Bind arrays to layout locations in shaders
     */
    inline void virtual bind() const override {
        IMeshGPU::bind();
        transforms_array_.attach_to_vertex_attribute(3);
    }

    /**
     * @brief The opposite of bind
     */
    inline void virtual release() const override {
        IMeshGPU::release();
        glDisableVertexAttribArray(3);
    }

    /**
     * @brief Get the transforms array
     *
     * @return const VertexBufferObject<glm::ivec4>& the transforms array
     */
    [[nodiscard]] inline const VertexBufferObject<glm::ivec4>&
    get_model_transforms() const noexcept {
        return transforms_array_;
    }

    /**
     * @brief Get the number of models
     *
     * @return uint32_t the number of models
     */
    [[nodiscard]] inline uint32_t virtual get_num_models() const noexcept override {
        return num_models_;
    }
};

} // namespace gpu_data

} // namespace gui
