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

#include <angelscript.h>
#include <sol/sol.hpp>

#include <expected>

class asContextWrapper;

class LocalContext {
 private:
    LocalContext();
    sol::state lua_state;

    asIScriptContext* context_;

    sol::object copy(sol::state& lua, const sol::object& object);

 public:
    [[nodiscard]] static LocalContext& instance();
    ~LocalContext();

    [[nodiscard]] inline sol::state&
    get_lua_state() {
        return lua_state;
    }

    std::optional<sol::object> get_from_this_lua_state(const std::string& command);

    void set_to_this_lua_state(const std::string& command, const sol::object& object);

    bool load_into_this_lua_state(const std::string& command);

    std::optional<sol::object> get_from_lua(const std::string& command);

    template <class T>
    std::expected<T, int>
    run_function(asIScriptFunction* function) {
        context_->Prepare(function);
        // set args maybe
        int result = context_->Execute();
        if (result != asEXECUTION_FINISHED) {
            return std::unexpected(result);
        }
        T object = *static_cast<T*>(context_->GetReturnObject());

        return object;
    }

    int
    run_function(asIScriptFunction* function) {
        context_->Prepare(function);
        // set args maybe
        int result = context_->Execute();
        return result;
    }

    // this might crash. should probably do some tests
    asIScriptObject* get_return_object() const {
        auto out = context_->GetAddressOfReturnValue();
        if (out == nullptr){
            return nullptr;
        }
        return *(asIScriptObject**)out;

    }

    int
    run_method(asIScriptObject* object, asIScriptFunction* function) {
        context_->Prepare(function);
        context_->SetObject(object);
        // set args maybe
        int result = context_->Execute();
        return result;
    }


};
