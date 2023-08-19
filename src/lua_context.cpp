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
 * @file lua_context.cpp
 *
 * @brief Defines Biome class.
 *
 * @ingroup Terrain Terrain_generation
 *
 */

#include "external_quill_logging.hpp"

#include <sol/sol.hpp>

void
add_logger(sol::state& lua) {
    LuaLogger& lua_logger = LuaLogger::getInstance();

    lua.set_function("log", &LuaLogger::log_info);
    lua.set_function("log_trace_l3", &LuaLogger::log_trace_l3, lua_logger);
    lua.set_function("log_trace_l2", &LuaLogger::log_trace_l2, lua_logger);
    lua.set_function("log_trace_l1", &LuaLogger::log_trace_l1, lua_logger);
    lua.set_function("log_debug", &LuaLogger::log_debug, lua_logger);
    lua.set_function("log_info", &LuaLogger::log_info, lua_logger);
    lua.set_function("log_warning", &LuaLogger::log_warning, lua_logger);
    lua.set_function("log_error", &LuaLogger::log_error, lua_logger);
    lua.set_function("log_critical", &LuaLogger::log_critical, lua_logger);
}
