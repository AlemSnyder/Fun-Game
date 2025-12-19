#pragma once

#include "bordered_widget.hpp"
#include "frame_part.hpp"
#include "gui/render/structures/window_texture.hpp"
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
};

} // namespace the_buttons

} // namespace gui