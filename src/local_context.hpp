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

class LocalContext {
 private:
    LocalContext();
    sol::state lua_state;

 public:
    // Delete all CTORs and CTOR-like operators
    LocalContext(LocalContext&&) = delete;
    LocalContext(LocalContext const&) = delete;

    void operator=(LocalContext&&) = delete;
    void operator=(LocalContext const&) = delete;

    [[nodiscard]] static LocalContext& instance();

    [[nodiscard]] inline static sol::state&
    get_lua_state() {
        return instance().lua_state;
    }
};
