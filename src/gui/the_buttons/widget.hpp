#pragma once

// #include "frame.hpp"
#include "../render/gpu_data/data_types.hpp"
#include "../scene/input.hpp"

namespace gui {

namespace the_buttons {

class UserInterface;

class WidgetInterface : public virtual scene::Inputs, public virtual gpu_data::GPUData {
 public:
    inline virtual ~WidgetInterface(){}; // kill children

    virtual bool is_interior(screen_size_t x, screen_size_t y) const = 0;

    inline virtual void render_children(const UserInterface* user_interface) const = 0;

    virtual bool has_children() const = 0;

    virtual std::weak_ptr<const WidgetInterface>
    get_child_at_position(screen_size_t x, screen_size_t y) const = 0;

    virtual std::array<screen_size_t, 4> get_bounding_box() const = 0;

    /**
     * @brief Handle key input including mouse keys
     *
     * @param GLFWwindow* window window event came from
     * @param int key GLFW key enum
     * @param int scancode GLFW scancode enum
     * @param int action GLFW action one of GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
     * @param int mods GLFW mods enum
     */
    virtual void handle_key_event_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key,
        [[maybe_unused]] int scancode, [[maybe_unused]] int action,
        [[maybe_unused]] int mods
    ) = 0;

    /**
     * @brief Handle text input
     *
     * @param GLFWwindow* window window to listen on
     * @param unsigned int codepoint unicode 32 character.
     */

    virtual void handle_text_input_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint
    ) = 0;

    /**
     * @brief Handle mouse movement events.
     *
     * @param GLFWwindow* window window to listen on
     * @param double xpos x position in window coordinates
     * @param double ypos y position in window coordinates.
     */
    virtual void handle_mouse_event_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xpos,
        [[maybe_unused]] double ypos
    ) = 0;

    /**
     * @brief Handle mouse enter window events
     *
     * @param GLFWwindow* window window to listen on
     * @param int button
     * @param int action
     * @param int mods
     */
    virtual void handle_mouse_button_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int button,
        [[maybe_unused]] int action, [[maybe_unused]] int mods
    ) = 0;

    /**
     * @brief Handle mouse scroll events
     *
     * @param GLFWwindow* window window to listen on
     * @param double xoffset x offset
     * @param double yoffset y offset (usually 0)
     */
    virtual void handle_mouse_scroll_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset,
        [[maybe_unused]] double yoffset
    ) = 0;

    /**
     * @brief Handle joystick event
     *
     * @param int jid joystick id
     * @param int event
     */
    virtual void
    handle_joystick_input([[maybe_unused]] int jid, [[maybe_unused]] int event) = 0;

    /**
     * @brief Handle file drop event
     *
     * @param GLFWwindow* window window to listen on
     * @param int count number of files passed
     * @param const char** paths file paths
     */
    virtual void handle_file_drop_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int count,
        [[maybe_unused]] const char** paths
    ) = 0;

    /**
     * @brief Handle all pooled inputs
     *
     * @param GLFWwindow* window window
     */
    virtual void handle_pooled_inputs([[maybe_unused]] GLFWwindow* window) = 0;

    /**
     * @brief Setup so this objects handles inputs correctly
     *
     * @param GLFWwindow* window window
     */
    virtual void setup([[maybe_unused]] GLFWwindow* window) = 0;

    /**
     * @brief Cleanup to original state
     *
     * @param GLFWwindow* window window
     */
    virtual void cleanup([[maybe_unused]] GLFWwindow* window) = 0;
};

template <typename T>
concept widget_type = std::is_base_of<WidgetInterface, T>::value;

} // namespace the_buttons

} // namespace gui
