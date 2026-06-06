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
 * @file text_widget.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines TextWidget Class
 *
 * @ingroup GUI  THE_BUTTONS
 *
 */

#pragma once

#include "bordered_widget.hpp"
#include "frame_part.hpp"
#include "global_context.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
#include "gui/render/structures/font.hpp"
#include "types.hpp"
#include "util/color.hpp"
#include "widget.hpp"

#include <memory>

namespace gui {

namespace the_buttons {

/**
 * @brief Widget for rendering text
 */
class TextWidget : public virtual WidgetBase, public virtual gpu_data::GPUDataElements {
 private:
    gpu_data::VertexArrayObject vertex_array_object_;

    std::shared_ptr<render::structures::FontTexture> font_;
    gpu_data::VertexBufferObject<glm::uvec4> text_data_;
    gpu_data::VertexBufferObject<
        uint16_t, gpu_data::BindingTarget::ELEMENT_ARRAY_BUFFER>
        element_array_;
    std::string text_;
    uint32_t num_characters_;

    // I guess all the text needs to be one color
    ColorFloat text_color_;

    bool wrap_text_;
    uint8_t text_scale_;

    void update_text_data();

    void initialize();

 protected:
    // attaches text_data_ to the vertex_array_object_
    void attach_all();

 public:
    TextWidget() = delete;
    /**
     * @brief Deleted copy constructor
     */
    TextWidget(const TextWidget& obj) = delete;

    /**
     * @brief Deleted copy operator
     */
    TextWidget& operator=(const TextWidget& obj) = delete;

    /**
     * @brief Default move constructor
     */
    TextWidget(TextWidget&& obj) = delete;

    /**
     * @brief Default move operator
     */
    TextWidget& operator=(TextWidget&& obj) = delete;

    /**
     * @brief Construct a new TextWidget object
     * @param WidgetInterface* parent Widget the be rendered relative to
     * @param std::shared_ptr<render::structures::FontTexture> font
     * @param glm::ivec2 position in parent widget to place text
     * @param glm::ivec2 widget_size Size of widget. Text will stay within this region
     * @param std::string text = ""
     * @param bool differed = true
     * @param bool wrap_text = true
     * @param uint8_t text_scale = 4
     * @param ColorFloat text_color = color::black
     */
    inline TextWidget(
        WidgetInterface* parent, std::shared_ptr<render::structures::FontTexture> font,
        glm::ivec2 position, glm::ivec2 widget_size, std::string text = std::string(""),
        bool differed = true, bool wrap_text = true, uint8_t text_scale = 4,
        ColorFloat text_color = color::black
    ) :
        WidgetBase(
            parent, position, widget_size,
            {position, glm::ivec2(position.x + widget_size.x, position.y),
             position + widget_size, glm::ivec2(position.x, position.y + widget_size.y)}
        ),
        vertex_array_object_(differed),

        font_(font), text_(text), num_characters_(text_.length()),
        text_color_(text_color), wrap_text_(wrap_text), text_scale_(text_scale) {
        if (differed) {
            GlobalContext& context = GlobalContext::instance();
            context.push_opengl_task([this]() { initialize(); });
        } else {
            initialize();
        }
        update_text_data();
    }

    inline virtual ~TextWidget() {};

    /**
     * @brief Set the display text to the given string
     *
     * @param std::string&& `text` text to display in the widget.
     */
    inline void
    set_text(std::string&& text) {
        text_ = text;
        num_characters_ = text_.length();
        update_text_data();
    }

    /**
     * @brief Get the number of vertices
     *
     * @return `unsigned int` the number of vertices. This is equal to six times the
     * number of characters.
     */
    inline unsigned int
    get_num_vertices() const {
        return num_characters_ * 6; // four vertices per character
    }

    /**
     * @brief Get the text color
     *
     * @return ColorFloat color of the text
     */
    inline ColorFloat
    get_text_color() const {
        return text_color_;
    }

    /**
     * @brief Get the element type. In this case I think it's `uint16_t`
     *
     * @return `gpu_data::GPUArayType::UNSIGNED_SHORT` (to the best of my knownage)
     */
    virtual gpu_data::GPUArayType
    get_element_type() const {
        return element_array_.get_opengl_numeric_type();
    }

    /**
     * @brief Bind OpenGL data to use in a program
     */
    inline virtual void
    bind() const {
        font_->bind(0);
        vertex_array_object_.bind();
        element_array_.bind();
    };

    /**
     * @brief Clears the OpenGL vertex array inputs
     */
    inline virtual void
    release() const {
        vertex_array_object_.release();
    }

    /**
     * @brief If the widget should be rendered
     *
     * @return bool true if the widget should be rendered, false otherwise.
     */
    inline virtual bool
    do_render() const {
        return true;
    };

    /**
     * @brief Called by the UI to render this widget to the screen
     *
     * @param UserInterface* user_interface The UserInterface that is rendering the
     * widget
     * @param screen_size_t x_position Position on the screen to render the widget
     * @param screen_size_t y_position Position on the screen to render the widget
     */
    void user_interface_render(
        const UserInterface* user_interface, screen_size_t x_position,
        screen_size_t y_position
    ) const override;

    /**
     * @brief Calculates the texture location to window space position for the given
     * text.
     *
     * @return std::vector<glm::uvec4> A vector of positions. The first two values in
     * the uvec4 are the position in the widget, and the last two are the position in
     * the texture.
     */
    [[nodiscard]] std::vector<glm::uvec4> generate_data() const;
};

} // namespace the_buttons

} // namespace gui