#include "../render/graphics_shaders/program_handler.hpp"
#include "../scene/helio.hpp"
#include "world/entity/object_handler.hpp"

#include <imgui/imgui.h>

#include <memory>

namespace gui {

namespace display_windows {

void display_data(
    std::map<const shader::ProgramData, shader::Program>& programs, bool& show
);

void display_data(std::shared_ptr<scene::Helio> helio, bool& show);

void display_data(world::entity::ObjectHandler& object_handler, bool& show);

} // namespace display_windows

} // namespace gui
