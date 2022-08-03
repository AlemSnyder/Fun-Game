// -*- lsst-c++ -*-
/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*/
/**
 * @file mesh.hpp
 *
 * @brief Defines Mesh class
 *
 * @ingroup Entity
 *
 */

#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>
#include <glm/glm.hpp>

/**
 * @brief Loads a mesh from a qb or compressed mesh into ram.
 * 
 */
class Mesh{
public:
    Mesh(const char* path);
    void get_mesh(std::vector<unsigned short> &indices,
                  std::vector<glm::vec3> &indexed_vertices,
                  std::vector<glm::vec3> &indexed_colors,
                  std::vector<glm::vec3> &indexed_normals) const;

    void load_from_qb(const char * path);
    // void load from smaller file
private:

    std::vector<int> size_;
    std::vector<int> center_;

    std::vector<std::uint16_t> indices_;
    std::vector<glm::vec3> indexed_vertices_;
    std::vector<glm::vec3> indexed_colors_;
    std::vector<glm::vec3> indexed_normals_;

    int get_position(int x, int y, int z) const ;

    void generate_mesh(std::vector<uint32_t> data,
                       std::vector<int> center,
                       std::vector<int> size);
};

#endif
