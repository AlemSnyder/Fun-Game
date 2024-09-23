#pragma once

#include "gui/render/graphics_shaders/shader_program.hpp"

#include <memory>

struct RenderPrograms {
    std::shared_ptr<gui::shader::ShaderProgram_ElementsInstanced> entity_render_program;
    std::shared_ptr<gui::shader::ShaderProgram_ElementsInstanced>
        tile_object_render_program;
};
