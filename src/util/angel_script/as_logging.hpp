// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file as_logging.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines functions that initialize AngelScript logging
 *
 */

#pragma once

#include <angelscript.h>

#include <string>

namespace util {
namespace scripting {

/**
 * @brief Initialize Logging interface on given engine
 *
 * @param asIScriptEngine* engine the engine logging will be initialized on.
 */
void init_as_interface(AngelScript::asIScriptEngine* engine);

} // namespace scripting

} // namespace util
