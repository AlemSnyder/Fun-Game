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
#include <logging.hpp>
#include <sol/sol.hpp>

#include <expected>

class asContextWrapper;

class LocalContext {
 private:
    LocalContext();
    sol::state lua_state;

    asIScriptContext* context_;

    sol::object copy(sol::state& lua, const sol::object& object);

    inline int
    set_arg(size_t i, bool arg) {
        return context_->SetArgByte(i, arg);
    }

    inline int
    set_arg(size_t i, int8_t arg) {
        return context_->SetArgByte(i, arg);
    }

    inline int
    set_arg(size_t i, uint8_t arg) {
        return context_->SetArgByte(i, arg);
    }

    inline int
    set_arg(size_t i, int16_t arg) {
        return context_->SetArgWord(i, arg);
    }

    inline int
    set_arg(size_t i, uint16_t arg) {
        return context_->SetArgWord(i, arg);
    }

    inline int
    set_arg(size_t i, int32_t arg) {
        return context_->SetArgDWord(i, arg);
    }

    inline int
    set_arg(size_t i, uint32_t arg) {
        return context_->SetArgDWord(i, arg);
    }

    inline int
    set_arg(size_t i, int64_t arg) {
        return context_->SetArgQWord(i, arg);
    }

    inline int
    set_arg(size_t i, uint64_t arg) {
        return context_->SetArgQWord(i, arg);
    }

    inline int
    set_arg(size_t i, float arg) {
        return context_->SetArgFloat(i, arg);
    }

    inline int
    set_arg(size_t i, double arg) {
        return context_->SetArgDouble(i, arg);
    }

    inline int
    set_arg(size_t i, void* arg) {
        return context_->SetArgObject(i, arg);
    }

    template <typename A>
    inline int
    set_all_args(const size_t i, const A&& a) {
        int result = set_arg(i, a);
        if (result != 0) {
            LOG_ERROR(
                logging::main_logger, "An error occurred {} at parameter {}", result, i
            );
        }
        return result;
    }

    template <typename A, typename... Args>
    inline int
    set_all_args(const size_t i, const A&& a, const Args&&... args) {
        int result = set_arg(i, a);
        if (result != 0) {
            LOG_ERROR(
                logging::main_logger, "An error occurred {} at parameter {}", result, i
            );
            return result;
        }
        return set_all_args(i + 1, std::forward<const Args>(args)...);
    }

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
        int result = context_->Execute();
        return result;
    }

    template <class... Args>
    inline int
    run_function(asIScriptFunction* function, const Args&&... args) {
        context_->Prepare(function);
        int result = set_all_args(0, std::forward<const Args>(args)...);
        if (result != 0) {
            return result;
        }
        result = context_->Execute();
        return result;
    }

    // this might crash. should probably do some tests
    asIScriptObject*
    get_return_object() const {
        auto out = context_->GetAddressOfReturnValue();
        if (out == nullptr) {
            return nullptr;
        }
        return *(asIScriptObject**)out;
    }

    int
    run_method(asIScriptObject* object, asIScriptFunction* function) {
        context_->Prepare(function);
        context_->SetObject(object);
        int result = context_->Execute();
        return result;
    }

    template <class... Args>
    inline int
    run_method(
        asIScriptObject* object, asIScriptFunction* function, const Args&&... args
    ) {
        context_->Prepare(function);
        context_->SetObject(object);
        int result = set_all_args(0, std::forward<const Args>(args)...);
        if (result != 0) {
            return result;
        }
        result = context_->Execute();
        return result;
    }

    // TODO add a check between the return signature of the current function and
    // the template type. Maybe in debug mode only
    //    template<typename T>
    //    int get_return_value(T& value) {
    //        auto address = context_->GetReturnAddress();

    //        if (address == nullptr) {
    //            return -1;
    //        }
    // good way to seg fault
    //        value = *((T*)address);
    //        return 0;
    //    }

    //    template<int>
    int
    get_return_value(int& value) {
        // if this is zero then could be right could be wrong.
        value = context_->GetReturnDWord();

        return 0;
    }

    inline int
    get_line_number(int stack_level, int* column, const char** section_name) const {
        return context_->GetLineNumber(stack_level, column, section_name);
    }
};
