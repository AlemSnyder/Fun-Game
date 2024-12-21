#pragma once

#include "gui/render/graphics_shaders/shader_program.hpp"

#include <memory>

// just an named tuple
// All the programs need to exist. This object verifies that shared pointers to
// the programs will exist and be unique.
struct render_program_t {
    std::shared_ptr<gui::shader::ShaderProgram_ElementsInstanced> entity_render_program;
    std::shared_ptr<gui::shader::ShaderProgram_ElementsInstanced>
        tile_object_render_program;
};
