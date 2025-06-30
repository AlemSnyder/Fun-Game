// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software
 *
 * Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file local_context.hpp
 *
 * @brief Defines Local Context class
 *
 * @details Creates a local Lua environment. Helpfull for multithreading.
 *
 * @ingroup --
 */

#pragma once

#include <sol/sol.hpp>

class GlobalCnotext;

class LocalContext {
 private:
    LocalContext();
    sol::state lua_state;

 public:
    [[nodiscard]] static LocalContext& get_local_context();

    [[nodiscard]] inline sol::state&
    get_lua_state() {
        return lua_state;
    }

    friend class GlobalContext;
};
