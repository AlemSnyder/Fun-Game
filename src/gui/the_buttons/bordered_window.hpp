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
 * @file bordered_window.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines BorderedWindow Class
 *
 * @ingroup GUI  THE_BUTTONS
 *
 */

#pragma once

#include "frame.hpp"
#include "gui/render/structures/window_texture.hpp"

#include <memory>

namespace gui {

namespace the_buttons {

class BorderedWindow : public virtual FrameBase {
 private:
    std::shared_ptr<render::WindowTexture> data_;

 public:
    BorderedWindow() = delete;
    /**
     * @brief Deleted copy constructor
     */
    BorderedWindow(const BorderedWindow& obj) = delete;

    /**
     * @brief Deleted copy operator
     */
    BorderedWindow& operator=(const BorderedWindow& obj) = delete;

    /**
     * @brief Default move constructor
     */
    BorderedWindow(BorderedWindow&& obj) = default;

    /**
     * @brief Default move constructor
     */
    BorderedWindow& operator=(BorderedWindow&& obj) = default;

    BorderedWindow(
        std::shared_ptr<render::WindowTexture> data, glm::ivec2 position,
        glm::ivec2 size
    );

    inline virtual ~BorderedWindow() {};

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
     * @brief Called by the UI to render this widget to the screen
     *
     * @param UserInterface* user_interface The UserInterface that is rendering the
     * widget
     * @param screen_size_t x_position Position on the screen to render the widget
     * @param screen_size_t y_position Position on the screen to render the widget
     */
    virtual void user_interface_render(
        const UserInterface* user_interface, screen_size_t x_position,
        screen_size_t y_position
    ) const override;
};

} // namespace the_buttons

} // namespace gui
