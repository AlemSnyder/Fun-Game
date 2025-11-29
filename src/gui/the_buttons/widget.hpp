#pragma once

#include "frame.hpp"
#include "gui/render/structures/window_texture.hpp"

namespace gui {
namespace the_buttons {
class Widget : public virtual Frame, public virtual gpu_data::ScreenData {};
} // namespace the_buttons
} // namespace gui