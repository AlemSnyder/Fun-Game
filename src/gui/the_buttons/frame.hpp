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
 * @file frame.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines FrameInterface and FrameBase Classes
 *
 * @ingroup GUI  THE_BUTTONS
 *
 */

#pragma once

#include "../render/gpu_data/data_types.hpp"
#include "gui/scene/input.hpp"
#include "types.hpp"
#include "widget.hpp"

#include <memory>
#include <unordered_set>
#include <vector>

namespace gui {

namespace the_buttons {

/**
 * @brief Interface for Frame. Frames hare top level widgets in the UI system. They don't have parents and are rendered relative to the window.
 */
class FrameInterface : public virtual WidgetInterface {
 public:
    inline virtual ~FrameInterface() {};

    /**
     * @brief Check if the position of the frame fixed on the screen
     * 
     * @return true if fixed false if not.
     */
    virtual bool is_fixed() const = 0;

    /**
     * @brief Check if the window should be rendered
     * 
     * @return false if should not be rendered true if should be
     */
    inline virtual bool is_visible() const = 0;

    /**
     * @brief Function called when the Frame is selected
     */
    virtual void on_select() = 0;

    /**
     * @brief Function called when any other frame is selected
     */
    virtual void on_end_select() = 0;

    /**
     * @brief Get the box that bounds of the widget.
     *
     * @details This should be the smallest rectangle that completely contains the
     * widget.
     *
     * @return std::array<screen_size_t, 4> [x position, y position, width, height]
     */
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

class FrameBase : public virtual FrameInterface {
 protected:
    glm::ivec2 position_;
    glm::ivec2 frame_size_;
    std::vector<glm::ivec2> exterior_points_;

    bool fixed_; // fixed position in render queue
    std::unordered_set<std::shared_ptr<WidgetInterface>> children_;

    bool is_selected;

 public:
    FrameBase(
        glm::ivec2 position, glm::ivec2 frame_size,
        std::vector<glm::ivec2> exterior_points, bool fixed = false
    ) :
        position_(position), frame_size_(frame_size), exterior_points_(exterior_points),
        fixed_(fixed) {};

    inline virtual ~FrameBase() {};

    /**
     * @brief Test if the given location is within the region of the widget
     *
     * @param screen_size_t x screen position x coordinate
     * @param screen_size_t y screen position y coordinate
     */
    bool is_interior(screen_size_t x, screen_size_t y) const;

    /**
     * @brief Get child widget if it exists at the given position.
     *
     * @param screen_size_t x screen position x coordinate
     * @param screen_size_t y screen position y coordinate
     *
     * @return std::weak_ptr<const WidgetInterface> a week pointer to the child widget.
     * It will exist if the given location has a child widget.
     */
    std::weak_ptr<const WidgetInterface>
    get_child_at_position(screen_size_t x, screen_size_t y) const;

    /**
     * @brief Quarry if this widget has child widgets.
     *
     * @return true if there are child widgets, false otherwise.
     */
    [[nodiscard]] inline virtual bool
    has_children() const override {
        return !children_.empty();
    }

    // prevent overlapping children.
    // bool check_children_positions();

    // might want to make it such that after a child has been added
    // nothing else can be added to it.
    // This will prevent circular references.
    // do this be checking if parent is set.
    // could also do the opposite and require that initialized with parent.

    /**
     * @brief Check if the position of the frame fixed on the screen
     * 
     * @return true if fixed false if not.
     */
    inline virtual bool
    is_fixed() const override {
        return fixed_;
    }

    /**
     * @brief Check if the window should be rendered
     * 
     * @return false if should not be rendered true if should be
     */
    inline virtual bool
    is_visible() const override {
        return true;
    }

    /**
     * @brief Function called when the Frame is selected
     */
    inline virtual void on_select() override {};

    /**
     * @brief Function called when any other frame is selected
     */
    inline virtual void on_end_select() override {};

    /**
     * @brief Get the box that bounds of the widget.
     *
     * @details This should be the smallest rectangle that completely contains the
     * widget.
     *
     * @return std::array<screen_size_t, 4> [x position, y position, width, height]
     */
    inline virtual std::array<screen_size_t, 4>
    get_bounding_box() const {
        return {position_.x, position_.y, frame_size_.x, frame_size_.y};
    }

    /**
     * @brief Construct a child widget of given type with given arguments.
     * @tparam widget_type T Child widget type
     * @tparam ...Args arguments for child widget
     * @param ...args arguments for child widget
     * @return std::shared_ptr<T> shared_ptr to child widget
     */
    template <widget_type T, class... Args>
    [[nodiscard]] inline std::shared_ptr<T>
    make(Args&&... args) {
        auto new_widget = children_.emplace(std::make_shared<T>(this, args...));
        if (new_widget.second) {
            if (auto new_widget_ptr = std::dynamic_pointer_cast<T>(*new_widget.first)) {
                return new_widget_ptr;
            }
        }
        return nullptr;
    }

    [[nodiscard]] inline auto
    begin() const {
        return children_.begin();
    }

    [[nodiscard]] inline auto
    end() const {
        return children_.end();
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

} // namespace the_buttons

} // namespace gui
