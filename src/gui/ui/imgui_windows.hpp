#include "../render/graphics_shaders/program_handler.hpp"
#include "../scene/helio.hpp"
#include "gui/scene/scene.hpp"
#include "world/entity/object_handler.hpp"

#include <imgui/imgui.h>

#include <memory>

namespace gui {

namespace display_windows {

/**
 * @brief Display program data
 *
 * @details Display compiled status and names of each program.
 *
 * @param std::map<const shader::ProgramData, shader::Program>& programs
 * programs map
 * @param bool& show reference to close window
 */
void display_data(
    std::map<const shader::ProgramData, shader::Program>& programs, bool& show
);

/**
 * @brief Display solar system data
 *
 * @param std::shared_ptr<scene::Helio> helio pointer to
 *
 * @param bool& show bool reference to close window
 */
void display_data(std::shared_ptr<scene::Helio> helio, bool& show);

void display_data(world::entity::ObjectHandler& object_handler, bool& show);

void display_data(Scene& scene, bool& show);

} // namespace display_windows

} // namespace gui
