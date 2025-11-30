#pragma once

#include "frame.hpp"
#include "gui/render/structures/screen_data.hpp"
#include "gui/render/structures/window_texture.hpp"

#include <memory>

namespace gui {

namespace the_buttons {

class BorderedWindow : public virtual Frame {
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

    BorderedWindow(std::shared_ptr<render::WindowTexture> data);

    inline virtual ~BorderedWindow(){};

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

    inline virtual std::array<screen_size_t, 4>
    get_bounding_box() const {
        return {70, 70, 120, 120};
    }
};

} // namespace the_buttons

} // namespace gui
