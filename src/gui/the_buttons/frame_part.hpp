#pragma once

#include "types.hpp"
#include "widget.hpp"

#include <memory>
#include <unordered_set>

namespace gui {

namespace the_buttons {

class WidgetBase : public virtual WidgetInterface {
 protected:
    WidgetInterface* parent_;
    glm::ivec2 position_;
    glm::ivec2 frame_size_;
    std::vector<glm::ivec2> exterior_points_;

    std::unordered_set<std::shared_ptr<WidgetInterface>> children;

    bool is_selected;

    void exterior_changed(); // need to change exterior for parent.

 public:
    WidgetBase(
        WidgetInterface* parent, glm::ivec2 position, glm::ivec2 frame_size,
        std::vector<glm::ivec2> exterior_points
    ) :
        parent_(parent),
        position_(position), frame_size_(frame_size),
        exterior_points_(exterior_points){};

    WidgetBase() = delete;
    /**
     * @brief Deleted copy constructor
     */
    WidgetBase(const WidgetBase& obj) = delete;

    /**
     * @brief Deleted copy operator
     */
    WidgetBase& operator=(const WidgetBase& obj) = delete;

    /**
     * @brief Default move constructor
     */
    WidgetBase(WidgetBase&& obj) = default;

    /**
     * @brief Default move constructor
     */
    WidgetBase& operator=(WidgetBase&& obj) = default;

    template <widget_type T, class... Args>
    [[nodiscard]] inline std::shared_ptr<T>
    make(Args&&... args) {
        auto new_widget = children.emplace(std::make_shared<T>(this, args...));
        if (new_widget.second) {
            if (auto new_widget_ptr = std::dynamic_pointer_cast<T>(*new_widget.first)) {
                return new_widget_ptr;
            }
        }
        return nullptr;
    }

    virtual bool is_interior(screen_size_t x, screen_size_t y) const override;

    // virtual void user_interface_render(
    //     const UserInterface* user_interface, screen_size_t x_position,
    //     screen_size_t y_position
    // ) const override;

    [[nodiscard]] virtual bool has_children() const override;

    virtual std::weak_ptr<const WidgetInterface>
    get_child_at_position(screen_size_t x, screen_size_t y) const override;

    inline virtual std::array<screen_size_t, 4>
    get_bounding_box() const override {
        return {position_.x, position_.y, frame_size_.x, frame_size_.y};
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
    virtual void handle_key_event_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key,
        [[maybe_unused]] int scancode, [[maybe_unused]] int action,
        [[maybe_unused]] int mods
    ) override{};

    /**
     * @brief Handle text input
     *
     * @param GLFWwindow* window window to listen on
     * @param unsigned int codepoint unicode 32 character.
     */

    virtual void handle_text_input_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint
    ) override{};

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
    ) override{};

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
    ) override{};

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
    ) override{};

    /**
     * @brief Handle joystick event
     *
     * @param int jid joystick id
     * @param int event
     */
    virtual void handle_joystick_input(
        [[maybe_unused]] int jid, [[maybe_unused]] int event
    ) override{};

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
    ) override{};

    /**
     * @brief Handle all pooled inputs
     *
     * @param GLFWwindow* window window
     */
    virtual void handle_pooled_inputs([[maybe_unused]] GLFWwindow* window) override{};

    /**
     * @brief Setup so this objects handles inputs correctly
     *
     * @param GLFWwindow* window window
     */
    virtual void setup([[maybe_unused]] GLFWwindow* window) override{};

    /**
     * @brief Cleanup to original state
     *
     * @param GLFWwindow* window window
     */
    virtual void cleanup([[maybe_unused]] GLFWwindow* window) override{};
};

} // namespace the_buttons

} // namespace gui
