#pragma once

#include "../render/gpu_data/data_types.hpp"
#include "gui/scene/input.hpp"
#include "types.hpp"

#include <memory>
#include <unordered_set>
#include <vector>

namespace gui {

namespace the_buttons {

class Frame : public virtual scene::Inputs, public virtual gpu_data::GPUData {
 protected:
    glm::ivec2 position_;
    glm::ivec2 frame_size_;
    std::vector<glm::ivec2> exterior_points_;

    Frame* parent_;
    bool fixed_; // fixed position in render queue
    std::unordered_set<std::shared_ptr<Frame>> children;
    void exterior_changed(); // need to change exterior for parent.

    bool is_selected;

 public:
    Frame(
        glm::ivec2 position, glm::ivec2 frame_size,
        std::vector<glm::ivec2> exterior_points, Frame* parent = nullptr,
        bool fixed = false
    ) :
        position_(position),
        frame_size_(frame_size), exterior_points_(exterior_points), parent_(parent),
        fixed_(fixed){};

    inline virtual ~Frame(){}; // kill children

    bool is_interior(screen_size_t x, screen_size_t y) const;

    std::weak_ptr<const Frame>
    get_child_at_position(screen_size_t x, screen_size_t y) const;

    [[nodiscard]] inline bool
    has_children() const {
        return !children.empty();
    }

    bool check_children();

    // private:
    // bool add_child(std::shared_ptr<Frame> child_frame);

    // might want to make it such that after a child has been added
    // nothing else can be added to it.
    // This will prevent circular references.
    // do this be checking if parent is set.
    // could also do the opposite and require that initialized with parent.

    inline bool
    is_fixed() const {
        return fixed_;
    }

    bool is_visible();

    void on_select();

    void on_end_select();
    virtual std::array<screen_size_t, 4> get_bounding_box() const = 0;

    [[nodiscard]] inline screen_size_t
    get_x_position() const {
        return position_.x;
    }

    [[nodiscard]] inline screen_size_t
    get_y_position() const {
        return position_.y;
    }

    [[nodiscard]] inline auto
    begin() const {
        return children.begin();
    }

    [[nodiscard]] inline auto
    end() const {
        return children.end();
    }

    /**
     * @brief Handle key input including mouse keys
     *
     * @param GLFWwindow* window window event came from
     * @param int key GLFW key enum
     * @param int scancode GLFW scancode enum
     * @param int action GLFW action one of GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
     * @param int mods GLFW mods enum
     */
    virtual void
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

// class UI_Text : public virtual Frame {
//  private:
//     std::string text_;
// };

// class Button : public virtual Frame {};

} // namespace the_buttons

} // namespace gui
