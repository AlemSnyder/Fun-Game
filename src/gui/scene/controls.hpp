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
 * @file controls.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines functions to get game position from inputs
 *
 * @ingroup CONTROLS
 *
 */

#pragma once

#include "input.hpp"
#include "keymapping.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace scene {
class Controls : public Inputs {
 protected:
    // KeyMapping key_map_;
    glm::mat4 view_matrix_;
    glm::mat4 projection_matrix_;

    int width_;
    int height_;
    // Initial position : on +Z
    glm::vec3 position_;
    // Initial horizontal angle : toward -Z
    float horizontal_angle_;
    // Initial vertical angle : none
    float vertical_angle_;
    // Initial Field of View
    float field_of_view_;

    float speed_; // units / second
    float mouse_speed_;

    double glfw_previous_time_;

    Key forward_key_;
    Key backward_key_;
    Key left_key_;
    Key right_key_;
    Key primary_interact_key_;
    Key secondary_interact_key_;
    Key up_key_;
    Key fast_key_;

 public:
    Controls(KeyMapping key_map) :
        width_(1), height_(1), position_(glm::vec3(80, 80, 80)),
        horizontal_angle_(3.14), vertical_angle_(1.57), field_of_view_(45.0),
        speed_(10), mouse_speed_(0.005), forward_key_(key_map[Action::FORWARD]),
        backward_key_(key_map[Action::BACKWARD]), left_key_(key_map[Action::LEFT]),
        right_key_(key_map[Action::RIGHT]),
        primary_interact_key_(key_map[Action::PRIMARY_INTERACT]),
        secondary_interact_key_(key_map[Action::SECONDARY_INTERACT]),
        up_key_(key_map[Action::UP]), fast_key_(key_map[Action::FAST]) {}

    /**
     * @brief Computes the view, and projection matrix using the size of the given
     * window
     *
     * @param window The current OpenGL window
     */
    virtual void handle_pooled_inputs(GLFWwindow* window);

    virtual void
    handle_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);

    virtual void setup(GLFWwindow* window);

    virtual void cleanup(GLFWwindow* window);

    /**
     * @brief Get the view matrix
     *
     * @return glm::mat4 projection of world space to camera space
     */
    [[nodiscard]] inline glm::mat4
    get_view_matrix() const {
        return view_matrix_;
    }

    /**
     * @brief Get the projection matrix
     *
     * @return glm::mat4 homographic projection of world onto view depending on the
     * FOV
     */
    [[nodiscard]] inline glm::mat4
    get_projection_matrix() const {
        return projection_matrix_;
    }

    /**
     * @brief Get the position vector
     *
     * @return glm::vec3 camera position in world
     */
    [[nodiscard]] inline glm::vec3
    get_position() const {
        return position_;
    }

    [[nodiscard]] inline glm::mat4
    get_inverse_view_projection() const {
        return glm::inverse(projection_matrix_ * view_matrix_);
    }
};

} // namespace scene

} // namespace gui
