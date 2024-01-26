// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file scene_setup.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines scene setup function
 *
 * @ingroup GUI
 *
 */

#pragma once

#include "../../world.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../scene/scene.hpp"

namespace gui {

void setup(Scene& scene, shader::ShaderHandler& shader_handler, World& world);

}
