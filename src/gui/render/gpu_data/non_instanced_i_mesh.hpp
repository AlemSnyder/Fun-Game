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
 * @file non_instanced_i_mesh.hpp
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

#include "array_buffer.hpp"
#include "entity/mesh.hpp"
#include "gpu_data.hpp"

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
class NonInstancedIMeshGPU : virtual public GPUDataElements {
 protected:
    ArrayBuffer<glm::ivec3> vertex_array_;
    ArrayBuffer<uint16_t> color_array_;
    ArrayBuffer<glm::i8vec3> normal_array_;
    ArrayBuffer<uint16_t, BindingTarget::ELEMENT_ARRAY_BUFFER> element_array_;
    GLuint color_texture_;
    uint32_t num_vertices_;
    bool do_render_;

 public:
    NonInstancedIMeshGPU(const NonInstancedIMeshGPU& other) = delete;
    NonInstancedIMeshGPU(NonInstancedIMeshGPU&& other) = default;
    // copy operator
    NonInstancedIMeshGPU& operator=(const NonInstancedIMeshGPU& other) = delete;
    NonInstancedIMeshGPU& operator=(NonInstancedIMeshGPU&& other) = default;

    inline NonInstancedIMeshGPU() : NonInstancedIMeshGPU(entity::Mesh()) {}

    explicit inline NonInstancedIMeshGPU(const entity::Mesh& mesh) :
        vertex_array_(mesh.get_indexed_vertices()),
        color_array_(mesh.get_indexed_color_ids()),
        normal_array_(mesh.get_indexed_normals()), element_array_(mesh.get_indices()),
        num_vertices_(mesh.get_indices().size()),
        do_render_(mesh.get_indices().size()) {}

    virtual void update(const entity::Mesh& mesh);

    virtual void bind() const;

    virtual void release() const;

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

    [[nodiscard]] inline GLuint
    get_color_texture() const noexcept {
        return color_texture_;
    }

    [[nodiscard]] inline uint32_t get_num_vertices() const noexcept override {
        return num_vertices_;
    }

    [[nodiscard]] inline GPUDataType get_element_type(
    ) const noexcept override {
        return element_array_.get_opengl_numeric_type();
    }
};

} // namespace gpu_data

} // namespace gui
