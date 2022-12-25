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
#pragma once

#include "./meshloader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

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
    GLuint programID_single_;   // ID of non-indexed mesh Program
    GLuint matrix_ID_;          // ID of world space to camera space transform matrix
    GLuint view_matrix_ID_;     // ID of view projection matrix
    GLuint depth_bias_ID_;      // ID of depth projection matrix
    GLuint shadow_map_ID_;      // ID of the shadow map
    GLuint color_map_ID_; // ID of the color map
    GLuint light_direction_ID_; // ID of the light direction uniform
    // -------------- indexed meshes -------------------
    GLuint programID_multi_; // ID of indexed mesh Program
    GLuint matrix_ID_multi_; // ID of world space to camera space transform matrix for
                             // indexed meshes
    GLuint view_matrix_ID_multi_; // ID of view projection matrix for indexed meshes
    GLuint depth_bias_ID_multi_;  // ID of depth projection matrix for indexed meshes
    GLuint shadow_map_ID_multi_;  // ID of the shadow map for indexed meshes
    GLuint color_map_ID_multi_;   // ID of the color map for indexed meshes
    GLuint light_direction_ID_multi_; // ID of the light direction uniform for indexed
                                      // meshes
    // ------ the below are added to the class ------
    GLuint depth_texture_;              // ID of the shadow depth texture
    glm::vec3 light_direction_;         // direction of sun light
    glm::mat4 depth_projection_matrix_; // projection matrix of the light source
    glm::mat4 depth_view_matrix_; // convert a point in world space to depth in light
                                  // direction
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
     * @param window the OpenGL window
     */
    void render(GLFWwindow* window) const;
};
