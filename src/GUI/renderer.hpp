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
 * @file render.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI
 *
 */

#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include <memory>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Renders the meshes to the screen
 * 
 * @details MainRenderer renders the meshes given to it to the screen.
 * this class handles the light direction, applied the meshes, and loading
 * shaders.
 * 
 */
class MainRenderer {
   private:
    GLuint programID_single_;    // ID of non-indexed mesh Program
    GLuint MatrixID_;            // def in class
    GLuint view_matrix_ID_;      // def in class
    GLuint DepthBiasID_;         // def in class
    GLuint ShadowMapID_;         // def in class
    GLuint light_direction_ID_;  // def in class

    GLuint programID_multi_;           // ID of indexed mesh Program
    GLuint MatrixID_multi_;            // def in class
    GLuint view_matrix_ID_multi_;      // def in class
    GLuint DepthBiasID_multi_;         // def in class
    GLuint ShadowMapID_multi_;         // def in class
    GLuint light_direction_ID_multi_;  // def in class

    GLuint depth_texture_;  // added to class
    // TODO all of these things should be defined somewhere else and sent to
    // this class.
    glm::vec3 light_direction_;          // direction of sun light
    //int windowFrameWidth;                // added to class
    //int windowFrameHeight;               // added to class
    glm::mat4 depth_projection_matrix_;  //! def in class
    glm::mat4 depth_view_matrix_;        // def in class
    std::vector<std::shared_ptr<MeshLoader::SingleComplexMesh>> singles_meshes_;
    std::vector<std::shared_ptr<MeshLoader::MultiComplexMesh>> multis_meshes_;

   public:
    /**
     * @brief Construct a new Main Renderer object
     * 
     */
    MainRenderer();

    ~MainRenderer();

    /**
     * @brief adds a non-indexed mesh so it will cast a shadow
     * 
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<MeshLoader::SingleComplexMesh> mesh);

    /**
     * @brief adds an indexed mesh so it will cast a shadow
     * 
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<MeshLoader::MultiComplexMesh> mesh);

    /**
     * @brief Set the depth texture ID
     * 
     * @param texture_id the depth texture ID
     */
    void set_depth_texture(GLuint texture_id);

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
     * @brief renders the given meshes
     * 
     * @param window 
     */
    void render(GLFWwindow* window) const;
};

#endif
