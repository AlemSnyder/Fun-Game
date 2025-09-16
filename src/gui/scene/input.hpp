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
 * @file input.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Inputs and InputHandler classes
 *
 * @ingroup GUI SCENE
 *
 */
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace gui {

namespace scene {

class Inputs {
 public:
    /**
     * @brief Handle key input including mouse keys
     *
     * @param GLFWwindow* window window event came from
     * @param int key GLFW key enum
     * @param int scancode GLFW scancode enum
     * @param int action GLFW action one of GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
     * @param int mods GLFW mods enum
     */
    void
    handle_key_event_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key,
        [[maybe_unused]] int scancode, [[maybe_unused]] int action,
        [[maybe_unused]] int mods
    ) {}

    /**
     * @brief Handle text input
     *
     * @param GLFWwindow* window window to listen on
     * @param unsigned int codepoint unicode 32 character.
     */

    virtual void
    handle_text_input_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint
    ) {}

    /**
     * @brief Handle mouse movement events.
     *
     * @param GLFWwindow* window window to listen on
     * @param double xpos x position in window coordinates
     * @param double ypos y position in window coordinates.
     */
    virtual void
    handle_mouse_event_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xpos,
        [[maybe_unused]] double ypos
    ) {}

    /**
     * @brief Handle mouse enter window events
     *
     * @param GLFWwindow* window window to listen on
     * @param int entered 1 if the curser entered the window, 0 if it exited.
     */
    virtual void
    handle_mouse_enter_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int entered
    ) {}

    /**
     * @brief Handle mouse enter window events
     *
     * @param GLFWwindow* window window to listen on
     * @param int button
     * @param int action
     * @param int mods
     */
    virtual void
    handle_mouse_button_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int button,
        [[maybe_unused]] int action, [[maybe_unused]] int mods
    ) {}

    /**
     * @brief Handle mouse scroll events
     *
     * @param GLFWwindow* window window to listen on
     * @param double xoffset x offset
     * @param double yoffset y offset (usually 0)
     */
    virtual void
    handle_mouse_scroll_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset,
        [[maybe_unused]] double yoffset
    ) {}

    /**
     * @brief Handle joystick event
     *
     * @param int jid joystick id
     * @param int event
     */
    virtual void
    handle_joystick_input([[maybe_unused]] int jid, [[maybe_unused]] int event) {}

    /**
     * @brief Handle file drop event
     *
     * @param GLFWwindow* window window to listen on
     * @param int count number of files passed
     * @param const char** paths file paths
     */
    virtual void
    handle_file_drop_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int count,
        [[maybe_unused]] const char** paths
    ) {}

    /**
     * @brief Handle all pooled inputs
     *
     * @param GLFWwindow* window window
     */
    virtual void
    handle_pooled_inputs([[maybe_unused]] GLFWwindow* window) {}

    /**
     * @brief Setup so this objects handles inputs correctly
     *
     * @param GLFWwindow* window window
     */
    virtual void
    setup([[maybe_unused]] GLFWwindow* window) {}

    /**
     * @brief Cleanup to original state
     *
     * @param GLFWwindow* window window
     */
    virtual void
    cleanup([[maybe_unused]] GLFWwindow* window) {}
};

/**
 * @brief Class that contains static functions to bind to glfw callbacks.
 */
class InputHandler {
    // somehow the plan was to only let GlobalContext set the window

 protected:
    // so realistically we would have a unordered map that maps the window
    // but im not making multiple windows
    static GLFWwindow* window_; // pointer to main window
    static std::shared_ptr<Inputs>
        forward_inputs_;         // Inputs object to forward keyboard inputs to
    static bool escape_pressed_; // flag to test if the escape key has peen pressed

    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void
    handle_key_event(GLFWwindow* window, int key, int scancode, int action, int mods);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void handle_text_input(GLFWwindow* window, unsigned int codepoint);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void handle_mouse_event(GLFWwindow* window, double xpos, double ypos);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void handle_mouse_enter(GLFWwindow* window, int enter);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void
    handle_mouse_button(GLFWwindow* window, int button, int action, int mods);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void handle_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void handle_joystick(int jid, int event);
    // takes input from GLFW and forwards it to Inputs. Dont call this directly.
    static void handle_file_drop(GLFWwindow* window, int count, const char** paths);

    // tests if the ImGui overlay wants to capture any inputs
    static bool imgui_capture();

    // inline InputHandler() : window_(0);
 public:
    // force static on class methods and variables
    InputHandler(InputHandler&&) = delete;
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(InputHandler&&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

    // sets window to the given window
    static void set_window(GLFWwindow* window);

    /**
     * @brief Forward user inputs to given Inputs interface
     */
    static void forward_inputs_to(std::shared_ptr<Inputs> forward_to);

    /**
     * @brief Handle currently pressed keys
     *
     * @param GLFWwindow* window window to handle events from
     */
    static void handle_pooled_inputs(GLFWwindow* window);

    // Tests if the escape key has been pressed
    inline static bool
    escape() {
        return escape_pressed_;
    }

    // need because escaping moves to imgui which won't forward the release of escape
    inline static void
    clear_escape() {
        escape_pressed_ = false;
    }
};

} // namespace scene

} // namespace gui
