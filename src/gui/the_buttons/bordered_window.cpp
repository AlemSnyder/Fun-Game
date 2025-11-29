#include "bordered_window.hpp"

namespace gui {

namespace the_buttons {

BorderedWindow::BorderedWindow(std::shared_ptr<render::WindowTexture> data) :
    data_(data) {}

} // namespace the_buttons
} // namespace gui