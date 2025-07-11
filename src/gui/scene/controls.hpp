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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

class GlobalContext;

namespace controls {

/**
 * @brief Computes the view, and projection matrix using the size of the given
 * window
 *
 * @param window The current OpenGL window
 */
void computeMatricesFromInputs(GLFWwindow* window);

/**
 * @brief Get the view matrix
 *
 * @return glm::mat4 projection of world space to camera space
 */
glm::mat4 get_view_matrix();

/**
 * @brief Get the projection matrix
 *
 * @return glm::mat4 homographic projection of world onto view depending on the
 * FOV
 */
glm::mat4 get_projection_matrix();

/**
 * @brief Get the position vector
 *
 * @return glm::vec3 camera position in world
 */
glm::vec3 get_position_vector();

/**
 * @brief True if the shadow map should be shown
 *
 * @param window current window
 * @return true the shadow map should be shown
 * @return false the shadow map should not be shown
 */
inline bool
show_shadow_map(GLFWwindow* window) {
    return glfwGetKey(window, GLFW_KEY_TAB);
}

class KeyEventCallback {
 public:
    KeyEventCallback();
    virtual ~KeyEventCallback();

    virtual void
    handle_input(GLFWwindow* window, int key, int scancode, int action, int mods) const;

 private:
};

// want a text input mode
//

class TextInputCallback {
 public:
    TextInputCallback();
    virtual ~TextInputCallback();
    /**
     * @brief handle text input
     *
     * @param GLFWwindow* window window to listen on
     * @param unsigned int codepoint unicode 32 charicter.
     */
    virtual bool handle_input(GLFWwindow* window, unsigned int codepoint) const;

 private:
};

class MouseEventCallback {
 public:
    MouseEventCallback();
    virtual ~MouseEventCallback();
    virtual void handle_input(GLFWwindow* window, double xpos, double ypos) const;
};

class MouseEnterCallback {
 public:
    MouseEnterCallback();
    virtual ~MouseEnterCallback();
    virtual void handle_input(GLFWwindow* window, int entered) const;
};

class MouseButtonCallback {
 public:
    MouseButtonCallback();
    virtual ~MouseButtonCallback();
    virtual void
    handle_input(GLFWwindow* window, int button, int action, int mods) const;
};

class MouseScrollCallback {
 public:
    MouseScrollCallback();
    virtual ~MouseScrollCallback();
    virtual void handle_input(GLFWwindow* window, double xoffset, double yoffset) const;
};

class JoystickCallback {
 public:
    JoystickCallback();
    virtual ~JoystickCallback();
    virtual void handle_input(int jid, int event) const;
};

class FileDropCallback {
 public:
    virtual ~FileDropCallback();
    virtual void handle_input(GLFWwindow* window, int count, const char** paths) const;
};

class PooledInputEvents {
 public:
    virtual ~PooledInputEvents();
    virtual void handle_input(GLFWwindow* windown) const;
};

class Inputs {
 protected:
    std::string name_;

 public:
    virtual std::shared_ptr<const KeyEventCallback> get_key_event_handler() const;
    virtual std::shared_ptr<const TextInputCallback> get_text_input_handler() const;
    virtual std::shared_ptr<const MouseEventCallback> get_mouse_event_handler() const;
    virtual std::shared_ptr<const MouseEnterCallback> get_mouse_enter_handler() const;
    virtual std::shared_ptr<const MouseButtonCallback> get_mouse_button_handler() const;
    virtual std::shared_ptr<const MouseScrollCallback> get_mouse_scroll_handler() const;
    virtual std::shared_ptr<const JoystickCallback> get_joystick_handler() const;
    virtual std::shared_ptr<const FileDropCallback> get_file_drop_handler() const;

    virtual std::shared_ptr<const PooledInputEvents> get_pooled_inputs_handler() const;

    virtual void setup(GLFWwindow* window) const;
    virtual void cleanup(GLFWwindow* window) const;
};

// how is this different from a namespace?
class InputHandler {
    friend GlobalContext;

 protected:
    // so realistically we would have a unordered map that maps the window
    static GLFWwindow* window_;
    static std::shared_ptr<const Inputs> forward_inputs_;
    static void
    handle_key_event(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void handle_text_input(GLFWwindow* window, unsigned int codepoint);
    static void handle_mouse_event(GLFWwindow* window, double xpos, double ypos);
    static void handle_mouse_enter(GLFWwindow* window, int enter);
    static void
    handle_mouse_button(GLFWwindow* window, int button, int action, int mods);
    static void handle_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);
    static void handle_joystick(int jid, int event);
    static void handle_file_drop(GLFWwindow* window, int count, const char** paths);

    static void handle_pooled_inputs(GLFWwindow* window);

    static bool imgui_capture();

    // inline InputHandler() : window_(0);
 public:
    InputHandler(InputHandler&&) = delete;
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(InputHandler&&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;
    ~InputHandler();

    static inline void set_window(GLFWwindow* window);

    static void forward_inputs_to(std::shared_ptr<const Inputs> forward_to);

 private:
};

// want a way to send general input
// is there anything else?

} // namespace controls
