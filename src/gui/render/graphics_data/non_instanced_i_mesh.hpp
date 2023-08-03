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
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

//! This is a "data" class. Its purpose is to send data to the gpu for that
// reason it should be moved into gui/data_structures
// Also no namespace terrain

#include "../../../entity/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace data_structures {

/**
 * @brief Class to render non-instanced meshes
 *
 * @details Handles non-instanced meshes. Sends mesh data to GPU, and handles
 * binding, and deleting data on GPU.
 */
class NonInstancedIMeshGPU {
 protected:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    GLuint color_texture_;
    uint32_t num_vertices_;
    bool do_render_;

 public:
    NonInstancedIMeshGPU(const NonInstancedIMeshGPU& other) = delete;
    NonInstancedIMeshGPU(NonInstancedIMeshGPU&& other) = default;
    // copy operator
    NonInstancedIMeshGPU& operator=(const NonInstancedIMeshGPU& other) = delete;
    NonInstancedIMeshGPU& operator=(NonInstancedIMeshGPU&& other) = default;

    inline NonInstancedIMeshGPU(){};
    explicit inline NonInstancedIMeshGPU(const entity::Mesh& mesh){
        update(mesh);
    }
    void update(const entity::Mesh& mesh);

    inline ~NonInstancedIMeshGPU() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
    }

    [[nodiscard]] inline bool
    do_render() const noexcept {
        return do_render_;
    }

    [[nodiscard]] inline GLuint
    get_color_buffer() const noexcept {
        return color_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_element_buffer() const noexcept {
        return element_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_normal_buffer() const noexcept {
        return normal_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_vertex_buffer() const noexcept {
        return vertex_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_color_texture() const noexcept {
        return color_texture_;
    }

    [[nodiscard]] inline unsigned int
    get_num_vertices() const noexcept {
        return num_vertices_;
    }
};

template <class T>
concept NonInstancedIMeshGPUDataType =
    std::is_base_of<gui::data_structures::NonInstancedIMeshGPU, T>::value;

} // namespace data_structures

} // namespace gui
