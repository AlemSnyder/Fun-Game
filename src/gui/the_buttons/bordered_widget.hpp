#pragma once

#include "bordered_widget.hpp"
#include "frame_part.hpp"
#include "gui/render/structures/window_texture.hpp"
#include "types.hpp"
#include "widget.hpp"

#include <memory>

namespace gui {

namespace the_buttons {

class BorderedWidget : public virtual WidgetBase {
 private:
    std::shared_ptr<render::WindowTexture> data_;

 public:
    BorderedWidget() = delete;
    /**
     * @brief Deleted copy constructor
     */
    BorderedWidget(const BorderedWidget& obj) = delete;

    /**
     * @brief Deleted copy operator
     */
    BorderedWidget& operator=(const BorderedWidget& obj) = delete;

    /**
     * @brief Default move constructor
     */
    BorderedWidget(BorderedWidget&& obj) = default;

    /**
     * @brief Default move constructor
     */
    BorderedWidget& operator=(BorderedWidget&& obj) = default;

    BorderedWidget(
        FrameInterface* parent, std::shared_ptr<render::WindowTexture> data,
        glm::ivec2 position, glm::ivec2 widget_size
    ) :
        WidgetBase(
            parent, position, widget_size,
            {position, glm::ivec2(position.x + widget_size.x, position.y),
             position + widget_size, glm::ivec2(position.x, position.y + widget_size.y)}
        ),
        // Frame(
        //     position, widget_size,
        //     {position, glm::ivec2(position.x + widget_size.x, position.y),
        //      position + widget_size,
        //      glm::ivec2(position.x, position.y + widget_size.y)},
        //     false
        // )

        data_(data) {
        data_->update_position(get_bounding_box());
    }

    inline virtual ~BorderedWidget(){};

    inline unsigned int
    get_num_vertices() const {
        return data_->get_num_vertices();
    }

    inline virtual void
    bind() const {
        data_->bind();
    };

    inline virtual void
    release() const {
        data_->release();
    }

    inline virtual bool
    do_render() const {
        return data_->do_render();
    };

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

    [[nodiscard]] inline virtual glm::ivec4
    get_border_size() const {
        return {5, 5, 5, 5};
    }

    [[nodiscard]] inline virtual glm::ivec4
    get_side_lengths() const {
        return {1, 1, 1, 1};
    }

    [[nodiscard]] inline virtual glm::ivec2
    get_inner_pattern_size() const {
        return {1, 1};
    }

    [[nodiscard]] inline virtual std::array<glm::ivec2, 9>
    get_texture_regions() const {
        return {glm::ivec2(0, 0), glm::ivec2(5, 0), glm::ivec2(6, 0),
                glm::ivec2(0, 5), glm::ivec2(5, 5), glm::ivec2(6, 5),
                glm::ivec2(0, 6), glm::ivec2(5, 6), glm::ivec2(6, 6)};
    }
};

} // namespace the_buttons

} // namespace gui