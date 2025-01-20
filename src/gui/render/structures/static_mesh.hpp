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
 * @file static_mesh.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines StaticMesh
 *
 * @ingroup GUI  DATA_STRUCTURED
 *
 */

#include "instanced_i_mesh.hpp"

#pragma once

namespace gui {

namespace gpu_data {

/**
 * @brief Instanced mesh with color texture generator for models
 *
 * @details Shallow derived class of InstancedInt. Color map is computed for
 * each mesh.
 */
class StaticMesh : public virtual InstancedIMeshGPU {
 protected:
    Texture1D color_texture_;

 public:
    inline StaticMesh(const world::entity::Mesh& mesh) : StaticMesh(mesh, {}) {}

    inline StaticMesh(
        const world::entity::Mesh& mesh, const std::vector<glm::ivec4>& model_transforms
    ) :
        InstancedIMeshGPU(mesh, model_transforms) {}

    inline void
    bind() const override {
        InstancedIMeshGPU::bind();
        color_texture_.bind(0);
    }

    inline virtual ~StaticMesh() { }
};

} // namespace gpu_data

} // namespace gui
