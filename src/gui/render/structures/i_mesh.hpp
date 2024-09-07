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
 * @file i_mesh.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines IndividualInt class
 *
 * @ingroup GUI  array_buffer
 *
 */

//! This is a "data" class. Its purpose is to send data to the gpu for that
// reason it should be moved into gui/array_buffer
// Also no namespace terrain

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/texture.hpp"
#include "gui/render/gpu_data/vertex_array_object.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
#include "world/entity/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace gpu_data {

/**
 * @brief Class to render non-instanced meshes
 *
 * @details Handles non-instanced meshes. Sends mesh data to GPU, and handles
 * binding, and deleting data on GPU.
 */
class IMeshGPU : virtual public GPUDataElements {
 protected:
    VertexArrayObject vertex_array_object_;

    VertexBufferObject<glm::ivec3> vertex_array_;
    VertexBufferObject<uint16_t> color_array_;
    VertexBufferObject<glm::i8vec3> normal_array_;
    VertexBufferObject<uint16_t, BindingTarget::ELEMENT_ARRAY_BUFFER> element_array_;
    uint32_t num_vertices_;
    bool do_render_;

 public:
    IMeshGPU(const IMeshGPU& other) = delete;
    IMeshGPU(IMeshGPU&& other) = default;
    // copy operator
    IMeshGPU& operator=(const IMeshGPU& other) = delete;
    IMeshGPU& operator=(IMeshGPU&& other) = default;

    inline IMeshGPU() :
        vertex_array_(), color_array_(), normal_array_(), element_array_(),
        num_vertices_(), do_render_() {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { initialize(); });
    }

    /**
     * @brief Construct a new IMeshGPU object
     *
     * @param world::entity::Mesh& mesh to load
     * @param bool b set to false when calling this constructor when inherited
     */
    explicit inline IMeshGPU(const world::entity::Mesh& mesh, bool b = true) :
        vertex_array_(mesh.get_indexed_vertices()),
        color_array_(mesh.get_indexed_color_ids()),
        normal_array_(mesh.get_indexed_normals()), element_array_(mesh.get_indices()),
        num_vertices_(mesh.get_indices().size()),
        do_render_(mesh.get_indices().size()) {
        if (b) {
            GlobalContext& context = GlobalContext::instance();
            context.push_opengl_task([this]() { initialize(); });
        }
    }

    /**
     * @brief Initializes Vertex Array Object.
     *
     * This might not have been the ideal design pattern. I may redo it later.
     */
    void initialize();

    /**
     * @brief Attach all Vertex Buffers to Layout positions on vertex and fragment
     * shaders.
     */
    virtual void attach_all();

    virtual void update(const world::entity::Mesh& mesh);

    virtual void bind() const override;

    virtual void release() const override;

    [[nodiscard]] inline bool virtual do_render() const noexcept override {
        return do_render_;
    }

    [[nodiscard]] inline const auto&
    get_color_buffer() const noexcept {
        return color_array_;
    }

    [[nodiscard]] inline const auto&
    get_element_buffer() const noexcept {
        return element_array_;
    }

    [[nodiscard]] inline const auto&
    get_normal_buffer() const noexcept {
        return normal_array_;
    }

    [[nodiscard]] inline const auto&
    get_vertex_buffer() const noexcept {
        return vertex_array_;
    }

    [[nodiscard]] inline uint32_t
    get_num_vertices() const noexcept override {
        return num_vertices_;
    }

    [[nodiscard]] inline GPUDataType
    get_element_type() const noexcept override {
        return element_array_.get_opengl_numeric_type();
    }
};

} // namespace gpu_data

} // namespace gui
