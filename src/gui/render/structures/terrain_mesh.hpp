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
 * @file terrain_mesh.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines TerrainMesh class
 *
 * @ingroup GUI  array_buffer
 *
 */

#pragma once

#include "i_mesh.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "util/chunk_hash.hpp"
#include "world/terrain/material.hpp"

namespace gui {

namespace gpu_data {

namespace detail {

struct coalesced_data {
    std::vector<glm::ivec3> vertex_array;
    std::vector<uint16_t> color_array;
    std::vector<glm::i8vec3> normal_array;

    std::vector<uint16_t> element_array;

    std::vector<GLsizei> num_vertices;
    std::vector<size_t> elements_offsets;
    std::vector<GLint> base_vertex;

    coalesced_data(const std::unordered_map<ChunkPos, world::entity::Mesh> mesh_map);
};

} // namespace detail

class IMeshMultiGPU : public virtual GPUDataElementsMulti {
 protected:
    VertexArrayObject vertex_array_object_;

    VertexBufferObject<glm::ivec3> vertex_array_;
    VertexBufferObject<uint16_t> color_array_;
    VertexBufferObject<glm::i8vec3> normal_array_;
    VertexBufferObject<uint16_t, BindingTarget::ELEMENT_ARRAY_BUFFER> element_array_;

    std::vector<GLsizei> num_vertices_;    // number of elements to read
    std::vector<size_t> elements_offsets_; // position of those elements
    std::vector<GLint> base_vertex_;       // off set into vertex array
    bool do_render_;

 public:
    IMeshMultiGPU(const IMeshMultiGPU& other) = delete;
    IMeshMultiGPU(IMeshMultiGPU&& other) = default;
    // copy operator
    IMeshMultiGPU& operator=(const IMeshMultiGPU& other) = delete;
    IMeshMultiGPU& operator=(IMeshMultiGPU&& other) = default;

    /**
     * @brief Construct a new NonInstancedIMeshMultiGPU object
     *
     * @details Default constructor
     */
    inline IMeshMultiGPU() :
        vertex_array_(), color_array_(), normal_array_(), element_array_(),
        num_vertices_(), do_render_() {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { initialize(); });
    }

    inline IMeshMultiGPU(const detail::coalesced_data data, bool b = true) :
        vertex_array_(data.vertex_array), color_array_(data.color_array),
        normal_array_(data.normal_array), element_array_(data.element_array),
        num_vertices_(data.num_vertices), elements_offsets_(data.elements_offsets),
        base_vertex_(data.base_vertex), do_render_(data.num_vertices.size()) {
        if (b) {
            GlobalContext& context = GlobalContext::instance();
            context.push_opengl_task([this]() { initialize(); });
        }
    }

    inline virtual ~IMeshMultiGPU() {}

    // if you cant construct it you can't hurt yourself

    /**
     * @brief Initializes Vertex Array Object.
     *
     * This might not have been the ideal design pattern. I may redo it later.
     */
    virtual void initialize();

    /**
     * @brief Attach all Vertex Buffers to Layout positions on vertex and fragment
     * shaders.
     */
    virtual void attach_all();

    size_t push_back(const world::entity::Mesh& mesh);

    void replace(size_t index, const world::entity::Mesh& mesh);

    void remove(size_t index);

    inline virtual void
    bind() const override {
        vertex_array_object_.bind();
    }

    inline virtual void
    release() const override {
        vertex_array_object_.release();
    }

    inline virtual bool
    do_render() const override {
        return do_render_;
    }

    inline virtual const std::vector<GLsizei>&
    get_num_vertices() const override {
        return num_vertices_;
    }

    inline virtual GPUDataType
    get_element_type() const override {
        return element_array_.get_opengl_numeric_type();
    }

    inline virtual uint32_t
    get_num_objects() const override {
        return num_vertices_.size();
    }

    inline virtual const std::vector<size_t>&
    get_elements_position() const override {
        return elements_offsets_;
    }

    inline virtual const std::vector<GLint>&
    get_base_vertex() const override {
        return base_vertex_;
    }

 private:
};

/**
 * @brief Array of all terrain mesh data. Also includes color texture for terrain.
 *
 * @details Manages chunk mesh data so that
 * glDrawElementsInstancedBaseVertexBaseInstance can be called. This function
 * uses only one opengl draw command.
 */
class TerrainMesh : public virtual IMeshMultiGPU {
 private:
    GLuint shadow_texture_;

    Texture1D& color_texture_;

    std::unordered_map<ChunkPos, size_t> world_position_to_index_;

 public:
    inline TerrainMesh() :
        color_texture_(terrain::TerrainColorMapping::get_color_texture()){};

    inline TerrainMesh(Texture1D& color_texture_id) :
        color_texture_(color_texture_id){};

    inline TerrainMesh(
        const std::unordered_map<ChunkPos, world::entity::Mesh> mesh_map,
        Texture1D& color_texture_id
    ) :
        IMeshMultiGPU(detail::coalesced_data(mesh_map), true),
        color_texture_(color_texture_id) {
        size_t index = 0;
        for (const auto& [pos, mesh] : mesh_map) {
            world_position_to_index_[pos] = index;
            index += 1;
        }
    }

    inline virtual ~TerrainMesh() {}

    inline void
    set_color_texture(Texture1D& color_texture_id) noexcept {
        color_texture_ = color_texture_id;
    }

    inline void
    set_shadow_texture(GLuint shadow_texture) {
        shadow_texture_ = shadow_texture;
    }

    void push_back(ChunkPos position, const world::entity::Mesh& mesh);

    void replace(ChunkPos position, const world::entity::Mesh& mesh);

    void remove(ChunkPos ChunkPos);

    inline void
    bind() const override {
        LOG_BACKTRACE(logging::opengl_logger, "Binding Terrain Mesh.");
        IMeshMultiGPU::bind();
        color_texture_.bind(0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadow_texture_);
    }
};

} // namespace gpu_data

} // namespace gui
