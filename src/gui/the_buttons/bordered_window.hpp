#pragma once

#include "gui/render/structures/screen_data.hpp"
#include "gui/render/structures/window_texture.hpp"
#include "widget.hpp"

#include <memory>

namespace gui {

namespace the_buttons {

class BorderedWindow : public virtual Widget {
 private:
    std::shared_ptr<render::WindowTexture> data_;

 public:
    BorderedWindow(std::shared_ptr<render::WindowTexture> data);
    inline virtual ~BorderedWindow(){};
};

} // namespace the_buttons

} // namespace gui
