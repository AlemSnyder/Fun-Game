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
#include "world/entity/placement.hpp"

#pragma once

namespace gui {

namespace data_structures {

/**
 * @brief Instanced mesh with color texture generator for models
 *
 * @details Shallow derived class of InstancedInt. Color map is computed for
 * each mesh.
 */
class StaticMesh : public virtual InstancedIMeshGPU {
 public:
    inline StaticMesh(const world::entity::Mesh& mesh) : StaticMesh(mesh, {}) {}

    inline StaticMesh(
        const world::entity::Mesh& mesh, const std::vector<glm::ivec4>& model_transforms
    ) :
        // what is this abomination?
        // TODO fix. This may be running twice I don't know.
        NonInstancedIMeshGPU(mesh),
        InstancedIMeshGPU(mesh, model_transforms) {
        generate_color_texture(mesh);
    }

    void update_position(
        uint offset, std::vector<glm::vec4> data
    );

 protected:
    void generate_color_texture(const world::entity::Mesh& mesh);
};

} // namespace data_structures

} // namespace gui
