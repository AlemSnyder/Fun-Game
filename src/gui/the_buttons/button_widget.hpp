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
 * @file button_widget.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines ButtonWidget Class
 *
 * @ingroup GUI  THE_BUTTONS
 *
 */

#pragma once

#include "bordered_widget.hpp"
#include "frame_part.hpp"
#include "gui/render/structures/window_texture.hpp"
#include "types.hpp"
#include "widget.hpp"

#include <functional>
#include <memory>

namespace gui {

namespace the_buttons {

/**
 * @brief Widget with the ability to call a function when it is "pressed"
 */
class ButtonWidget : public virtual WidgetBase {
 private:
    std::shared_ptr<render::WindowTexture> data_;

    std::function<void()> button_function_;

 public:
    ButtonWidget() = delete;
    /**
     * @brief Deleted copy constructor
     */
    ButtonWidget(const ButtonWidget& obj) = delete;

    /**
     * @brief Default copy operator
     */
    ButtonWidget& operator=(const ButtonWidget& obj) = default;

    /**
     * @brief Default move constructor
     */
    ButtonWidget(ButtonWidget&& obj) = delete;
    // TODO do this for all virtual classes
    // ButtonWidget(ButtonWidget&& obj) {
    //     static_cast<WidgetBase&>(*this) = std::move(static_cast<WidgetBase&>(obj));
    //     data_ = std::move(obj.data_);
    //     button_function_ = std::move(button_function_);
    // }

    /**
     * @brief Default move constructor
     */
    ButtonWidget& operator=(ButtonWidget&& obj) = delete;

    /**
     * @brief Construct a new ButtonWidget object.
     * @param WidgetInterface* parent Parent Widget. Will be rendered on top of and
     * relative to parent
     * @param std::shared_ptr<render::WindowTexture> data OpenGL texture of the button
     * @param glm::ivec2 position Position in parent widget
     * @param glm::ivec2 widget_size size of widget
     * @param std::function<void()> button_function Function called when button is
     * pressed.
     */
    ButtonWidget(
        WidgetInterface* parent, std::shared_ptr<render::WindowTexture> data,
        glm::ivec2 position, glm::ivec2 widget_size,
        std::function<void()> button_function
    ) :
        WidgetBase(
            parent, position, widget_size,
            {position, glm::ivec2(position.x + widget_size.x, position.y),
             position + widget_size, glm::ivec2(position.x, position.y + widget_size.y)}
        ),

        data_(data), button_function_(button_function) {
        data_->update_position(get_bounding_box());
    }

    inline virtual ~ButtonWidget() {};

    /**
     * @brief Get number of vertices on path that surrounds this widget.
     * @return unsigned int the number of vertices
     */
    inline unsigned int
    get_num_vertices() const {
        return data_->get_num_vertices();
    }

    /**
     * @brief Bind OpenGL data to use in a program
     */
    inline virtual void
    bind() const {
        data_->bind();
    };

    /**
     * @brief Clears the OpenGL vertex array inputs
     */
    inline virtual void
    release() const {
        data_->release();
    }

    /**
     * @brief If the widget should be rendered
     *
     * @return bool true if the widget should be rendered, false otherwise.
     */
    inline virtual bool
    do_render() const {
        return data_->do_render();
    };

    /**
     * @brief Called by the UI to render this widget to the screen
     *
     * @param UserInterface* user_interface The UserInterface that is rendering the
     * widget
     * @param screen_size_t x_position Position on the screen to render the widget
     * @param screen_size_t y_position Position on the screen to render the widget
     */
    inline void
    user_interface_render(
        const UserInterface* user_interface, screen_size_t x_position,
        screen_size_t y_position
    ) const override {
        user_interface->render_frame(this, x_position, y_position);

        for (const auto& child : children) {
            child->user_interface_render(
                user_interface, x_position + position_.x, y_position + position_.y
            );
        }
    }

    /**
     * @brief Size of the four borders. Used in uniforms
     */
    [[nodiscard]] inline virtual glm::ivec4
    get_border_size() const {
        return data_->get_border_size();
    }

    /**
     * @brief Size of the four side lengths. Used in uniforms
     */
    [[nodiscard]] inline virtual glm::ivec4
    get_side_lengths() const {
        return data_->get_side_lengths();
    }

    /**
     * @brief Size of the four borders. Used in uniforms
     */
    [[nodiscard]] inline virtual glm::ivec2
    get_inner_pattern_size() const {
        return data_->get_inner_pattern_size();
    }

    /**
     * @brief Size of the four borders. Used in uniforms
     */
    [[nodiscard]] inline virtual std::array<glm::ivec2, 9>
    get_texture_regions() const {
        return data_->get_texture_regions();
    }

    // rn this may work, but...
    // want to separate between has clickable and renderable children.
    /**
     * @brief Quarry if this widget has child widgets.
     *
     * @return true if there are child widgets, false otherwise.
     */
    inline bool
    has_children() const {
        return false;
    }

    inline virtual void
    handle_mouse_button_input(
        [[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int button,
        [[maybe_unused]] int action, [[maybe_unused]] int mods
    ) {
        if (action == GLFW_PRESS) {
            button_function_();
        }
    }
};

} // namespace the_buttons

} // namespace gui