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
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

//#include "shadow_map.hpp"
#include "non_instanced_i_mesh.hpp"

namespace gui {

namespace data_structures {

/**
 * @brief Non-instanced mesh with color texture for terrain
 *
 * @details Shallow derived class of IndividualInt. Color texture is made
 * for all materials in the terrain.
 */
class TerrainMesh : public virtual NonInstancedIMeshGPU {
 private:
    GLuint shadow_texture_;

 public:
    inline TerrainMesh(){};

    inline TerrainMesh(const world::entity::Mesh& mesh, GLuint color_texture_id) :
        NonInstancedIMeshGPU(mesh) {
        set_color_texture(color_texture_id);
    }

    inline void
    set_color_texture(GLuint color_texture_id) noexcept {
        color_texture_ = color_texture_id;
    }

    inline void set_shadow_texture(GLuint shadow_texture){
        shadow_texture_ = shadow_texture;
    }

    inline virtual void
    bind() const override {
        NonInstancedIMeshGPU::bind();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadow_texture_);
    }
};

} // namespace data_structures

} // namespace gui
