#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

class GlobalContext;

namespace gui {

namespace scene {

class Inputs {
 public:
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

// how is this different from a namespace?
class InputHandler {
    friend GlobalContext;

 protected:
    // so realistically we would have a unordered map that maps the window
    static GLFWwindow* window_;
    static std::shared_ptr<Inputs> forward_inputs_;
    static bool escape_pressed_;

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

    static bool imgui_capture();

    // inline InputHandler() : window_(0);
 public:
    InputHandler(InputHandler&&) = delete;
    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(InputHandler&&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;
    ~InputHandler();

    static void set_window(GLFWwindow* window);

    static void forward_inputs_to(std::shared_ptr<Inputs> forward_to);

    static void handle_pooled_inputs(GLFWwindow* window);

    inline static bool
    escape() {
        return escape_pressed_;
    }

 private:
};

// class world space controls : virtual Inputs
// owns a bunch of glm objects describing where we are
// owns start times for the important keys
// constructor from maybe a file of keymaps
// define set up, and cleanup
// define key event
// define mouse event (movement)
// define mouse button handler
// define mouse scroll handler
// define an update that moves the position even if there is no call back

} // namespace scene

} // namespace gui
