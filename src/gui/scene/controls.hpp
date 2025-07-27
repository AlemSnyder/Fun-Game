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
 * @brief Defines Controls class that handles any sort of user input.
 *
 * @ingroup GUI SCENE
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

/**
 * @brief Defines changes to be taken because of user input
 */
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

    KeyMapping key_map_;

 public:
    /**
     * @brief Construct a new Controls object
     *
     * @param KeyMapping key_map map from keyboard keys to actions.
     */
    Controls(KeyMapping key_map) :
        width_(1), height_(1), position_(glm::vec3(80, 80, 80)),
        horizontal_angle_(3.14), vertical_angle_(1.57), field_of_view_(45.0),
        speed_(10), mouse_speed_(0.005), key_map_(key_map) {}

    /**
     * @brief Updates the scene based on all user inputs.
     *
     * @param window The current OpenGL window
     */
    virtual void handle_pooled_inputs(GLFWwindow* window) override;

    /**
     * @brief Handle Mouse Wheel Scroll Events
     */
    virtual void handle_mouse_scroll_input(
        GLFWwindow* window, double xoffset, double yoffset
    ) override;

    virtual void
    handle_mouse_button_input(
        GLFWwindow* window, int button,
        int action, int mods
    );

    /**
     * @brief To be called when controls are bound. (hide curser etc)
     *
     * @warning Won't play nice with ImGUI
     */
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

    /**
     * @brief Get view inverse projection
     *
     * @details Used for star projection
     */
    [[nodiscard]] inline glm::mat4
    get_inverse_view_projection() const {
        return glm::inverse(projection_matrix_ * view_matrix_);
    }

    [[nodiscard]] inline int
    get_width() const {
        return width_;
    }
    
    [[nodiscard]] inline int
    get_height() const {
        return height_;
    }
};

} // namespace scene

} // namespace gui
