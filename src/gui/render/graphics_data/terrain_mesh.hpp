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

#include "individual_int.hpp"

namespace gui {

namespace data_structures {

/**
 * @brief Non-instanced mesh with color texture for terrain
 *
 * @details Shallow derived class of IndividualInt. Color texture is made
 * for all materials in the terrain. 
 */
class TerrainMesh : public NonInstancedIMeshGPU {
 public:
    TerrainMesh();
    TerrainMesh(const entity::Mesh& mesh, GLuint color_texture);

    void set_color_texture(GLuint color_texture) noexcept;

};

}

}
