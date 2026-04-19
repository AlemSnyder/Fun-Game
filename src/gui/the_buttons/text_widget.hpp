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
     * @brief Default move constructor
     */
    TextWidget& operator=(TextWidget&& obj) = delete;

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
        wrap_text_(wrap_text), text_color_(text_color), text_scale_(text_scale) {
        if (differed) {
            GlobalContext& context = GlobalContext::instance();
            context.push_opengl_task([this]() { initialize(); });
        } else {
            initialize();
        }
        update_text_data();
    }

    inline virtual ~TextWidget() {};

    void attach_all();

    inline void
    set_text(std::string&& text) {
        text_ = text;
        num_characters_ = text_.length();
        update_text_data();
    }

    inline unsigned int
    get_num_vertices() const {
        return num_characters_ * 6; // four vertices per character
        // return 12;
    }

    inline ColorFloat
    get_text_color() const {
        return text_color_;
    }

    virtual gpu_data::GPUArayType
    get_element_type() const {
        return element_array_.get_opengl_numeric_type();
    }

    inline virtual void
    bind() const {
        font_->bind(0);
        vertex_array_object_.bind();
        element_array_.bind();
    };

    inline virtual void
    release() const {
        vertex_array_object_.release();
    }

    inline virtual bool
    do_render() const {
        return true;
    };

    void user_interface_render(
        const UserInterface* user_interface, screen_size_t x_position,
        screen_size_t y_position
    ) const override;

    [[nodiscard]] std::vector<glm::uvec4> generate_data() const;
};

} // namespace the_buttons

} // namespace gui