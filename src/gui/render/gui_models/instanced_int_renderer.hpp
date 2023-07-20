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

#include "../../shader.hpp"
#include "../data/instanced_int.hpp"
#include "gui_render_types.hpp"
#include "individual_int_renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

namespace gui {

namespace models {

/**
 * @brief Renders the meshes to the screen
 *
 * @details MainRenderer renders the meshes given to it to the screen.
 * this class handles the light direction, applied the meshes, and loading
 * shaders.
 *
 */

//template <gui::data_structures::InstancedIntLike T>
template <class T>
class InstancedIntRenderer : public IndividualIntRenderer<T> {
 private:
    // forgot what to put here
 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    InstancedIntRenderer(ShaderHandeler shader_handeler = ShaderHandeler());

    ~InstancedIntRenderer();

    void load_transforms_buffer(std::shared_ptr<T> mesh) const;

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     */
    void render_frame_buffer(GLFWwindow* window, GLuint frame_buffer = 0) const;

    void
    render_frame_buffer_multisample(GLFWwindow* window, GLuint frame_buffer = 0) const;

    void
    render_shadow_map(int shadow_width_, int shadow_height_, GLuint frame_buffer) const;
};

} // namespace models

} // namespace gui
