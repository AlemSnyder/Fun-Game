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
 * @file imgui_gui.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines imgui_entry function
 *
 * @ingroup
 *
 */

#include "../../world.hpp"

namespace gui {
/**
 * @brief Runs the gui using imgui.
 *
 * @details The frame buffer is rendered to an imgui window. Other imgui frames
 * can be used to interface with data.
 *
 * @returns int exit status to be sent to main.
 */
int imgui_entry(World& world);

} // namespace gui