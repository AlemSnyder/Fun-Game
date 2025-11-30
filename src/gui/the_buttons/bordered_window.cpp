#include "bordered_window.hpp"

namespace gui {

namespace the_buttons {

BorderedWindow::BorderedWindow(std::shared_ptr<render::WindowTexture> data) :
    data_(data) {
    data_->update_position(get_bounding_box());
}

} // namespace the_buttons
} // namespace gui