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
 * @file shadow_map.hpp
 *
 * @brief Defines ShadowMap class.
 *
 * @ingroup GUI
 *
 */

#ifndef __GUI_SHADOW_MAP_HPP__
#define __GUI_SHADOW_MAP_HPP__

#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "meshloader.hpp"
#include "shader.hpp"

/**
 * @brief Renders the shadow from the given meshes.
 * 
 * @details ShadowMap renders the depth texture to depthTexture. First sets the
 * meshes to render from, then renders the depth using the given light
 * direction, and projection matrix.
 */
class ShadowMap {
   private:
    GLuint programID_;                  // ID of non-indexed mesh Program
    GLuint depth_matrix_ID_;            // ID of depth matrix for non-indexed Program
    GLuint programID_multi_;            // ID of indexed mesh Program
    GLuint depth_matrix_ID_multi_;      // ID of depth matrix for indexed Program
    GLuint depth_texture_;              // ID of depth texture
    GLuint frame_buffer_name_;          // ID of frame buffer
    // ------ the below are added to the class ------
    glm::vec3 light_direction_;         // direction of sunlight
    uint32_t shadow_width_;             // width of depth texture
    uint32_t shadow_height_;            // height of depth texture
    glm::mat4 depth_projection_matrix_; // projection matrix of the light source
    glm::mat4 depth_view_matrix_;       // convert a point in world space to depth in light direction
    // vector of stored data that describes non-indexed meshes
    std::vector<std::shared_ptr<MeshLoader::SingleMesh>> singles_meshes_;
    // vector of stored data that describes indexed meshes
    std::vector<std::shared_ptr<MeshLoader::MultiMesh>> multi_meshes_;

   public:
    /**
     * @brief Construct a new Shadow Map object
     * 
     * @param w the width of the area hit by light
     * @param h the height of the area hit by light
     */
    ShadowMap(int w, int h);

    /**
     * @brief adds a non-indexed mesh so it will cast a shadow
     * 
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<MeshLoader::SingleMesh> mesh);

    /**
     * @brief adds an indexed mesh so it will cast a shadow
     * 
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<MeshLoader::MultiMesh> mesh);

    /**
     * @brief Get the depth texture ID
     * 
     * @return GLuint& reference to depth texture ID
     */
    inline GLuint &get_depth_texture() { return depth_texture_; }

    /**
     * @brief Get the frame buffer ID
     * 
     * @return GLuint& reference to frame buffer ID
     */
    inline GLuint &et_frame_buffer() { return frame_buffer_name_; }

    /**
     * @brief Set the light direction vector
     * 
     * @param light_direction the direction of the light
     */
    void set_light_direction(glm::vec3 light_direction);

    /**
     * @brief Set the depth projection matrix
     * 
     * @param depth_projection_matrix the projection matrix
     */
    void set_depth_projection_matrix(glm::mat4 depth_projection_matrix);

    /**
     * @brief renders the length of the light ray to the depth buffer
     * 
     */
    void render_shadow_depth_buffer() const;
};

#endif
