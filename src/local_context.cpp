#include "local_context.hpp"

#include "global_context.hpp"
#include "util/lua/lua_logging.hpp"
#include "world/terrain/generation/lua_interface.hpp"

// #include <bits/stdc++>

LocalContext::LocalContext() {
    lua_state.open_libraries(sol::lib::base);
    lua_state.open_libraries(sol::lib::math);
    lua_state.open_libraries(sol::lib::string);
    lua_state.open_libraries(sol::lib::debug);
    lua_logging::setup_lua_logging(lua_state);
    terrain::generation::init_lua_interface(lua_state);
}

LocalContext&
LocalContext::instance() {
    static thread_local LocalContext local_ctx;
    return local_ctx;
}

std::optional<sol::object>
LocalContext::get_from_this_lua_state(const std::string& command) {
    LOG_BACKTRACE(logging::lua_logger, "Attempting to index {}.", command);
    sol::state& lua = get_lua_state();

    std::stringstream command_stream(command);

    std::string key;

    std::getline(command_stream, key, '\\');

    auto raw_result = lua.get<sol::optional<sol::object>>(key);

    if (!raw_result) {
        LOG_BACKTRACE(logging::lua_logger, "{} not valid.", key);
        return {};
    }

    sol::table result;

    while (std::getline(command_stream, key, '\\')) {
        if (!raw_result->is<sol::table>()) {
            LOG_BACKTRACE(logging::lua_logger, "{} not index of table.", key);
            return {};
        }
        result = raw_result.value();

        if (!result.valid()) {
            LOG_BACKTRACE(logging::lua_logger, "Could not find {}.", key);
            return {};
        }

        if (result == sol::lua_nil) {
            LOG_BACKTRACE(
                logging::lua_logger, "Attempting to index {}. nil value at {}.",
                command, key
            );
            return {};
        }

        if (!result.is<sol::table>()) {
            LOG_BACKTRACE(
                logging::lua_logger, "Attempting to index {}. {} not index of table.",
                command, key
            );
            return {};
        }

        raw_result = result.get<sol::optional<sol::object>>(key);

        if (!raw_result) {
            LOG_BACKTRACE(logging::lua_logger, "{} not valid.", key);
            return {};
        }
        LOG_BACKTRACE(logging::lua_logger, "{} valid.", key);
    }

    // a sol object
    return raw_result.value();
}

void
LocalContext::set_to_this_lua_state(
    const std::string& command, const sol::object& object
) {
    if (command.length() == 0) {
        LOG_WARNING(logging::lua_logger, "Cannot set table with no name.");
        return;
    }
    LOG_BACKTRACE(logging::lua_logger, "Attempting to index {}.", command);
    sol::state& lua = get_lua_state();

    std::stringstream command_stream(command);
    std::string key;

    sol::optional<sol::object> raw_result;

    while (std::getline(command_stream, key, '\\')) {
        LOG_BACKTRACE(logging::lua_logger, "Locking up {}.", key);
        // if last part of command
        if (command_stream.tellg() == -1) {
            if (raw_result) {
                if (raw_result->is<sol::table>()) {
                    sol::table result = raw_result.value();
                    result.set(key, object);
                } else {
                    LOG_ERROR(logging::lua_logger, "Object at {} is not a table.", key);
                }

            } else {
                lua.set(key, object);
            }
            continue;
            // will return here
        }
        if (raw_result) {
            if (raw_result->is<sol::table>()) {
                sol::table result = raw_result.value();
                raw_result = result.get<sol::optional<sol::table>>(key);
                if (!raw_result) {
                    result.set(key, lua.create_table());
                    raw_result = result.get<sol::optional<sol::table>>(key);
                }
            } else {
                LOG_ERROR(logging::lua_logger, "Object at {} is not a table.", key);
            }
        } else {
            raw_result = lua.get<sol::optional<sol::object>>(key);
            if (!raw_result) {
                lua.set(key, lua.create_table());
                raw_result = lua.get<sol::optional<sol::table>>(key);
            }
        }
        if (!raw_result) {
            LOG_ERROR(logging::lua_logger, "Could not find key {}", key);
            return;
        }
    }
}

bool
LocalContext::load_into_this_lua_state(const std::string& command) {
    GlobalContext& context = GlobalContext::instance();
    std::optional<sol::object> object_result = context.get_from_lua(command);

    if (!object_result) {
        LOG_WARNING(logging::lua_logger, "Command {} not found.", command);
        return false;
    }

    sol::object object_this_state = copy(lua_state, object_result.value());

#if DEBUG()
    if (object_this_state.is<sol::table>()) {
        sol::table table_copy = object_this_state;

        for (auto& [key, value] : table_copy) {
            LOG_DEBUG(logging::lua_logger, "key: {}", key.as<std::string>());
        }
    }
#endif

    set_to_this_lua_state(command, object_this_state);
    return true;
}

sol::object
LocalContext::copy(sol::state& lua_state, const sol::object& object) {
    sol::type type = object.get_type();
    switch (type) {
        case sol::type::number:
            {
                double number = object.as<double>();
                return sol::make_object(lua_state, number);
            }
        case sol::type::string:
            {
                std::string string = object.as<std::string>();
                return sol::make_object(lua_state, string);
            }
        case sol::type::boolean:
            {
                bool b = object.as<bool>();
                return sol::make_object(lua_state, b);
            }
        case sol::type::table:
            {
                sol::table table = object.as<sol::table>();
                sol::table table_copy = lua_state.create_table();
                for (auto& [key, value] : table) {
                    table_copy.set(copy(lua_state, key), copy(lua_state, value));
                }
                return table_copy;
            }
        case sol::type::function:
            {
                sol::protected_function funct = object.as<sol::protected_function>();
                sol::bytecode target_bc = funct.dump();
                auto result = lua_state.load(target_bc.as_string_view());
                sol::protected_function loaded_function = result;
                return loaded_function;
            }
        case sol::type::lua_nil:
        case sol::type::none:
            return {};
        default:
            return {};
    }
}

std::optional<sol::object>
LocalContext::get_from_lua(const std::string& command) {
    std::optional<sol::object> in_this_state = get_from_this_lua_state(command);
    if (in_this_state) {
        return in_this_state;
    }
    bool load_secsess = load_into_this_lua_state(command);
    if (!load_secsess) {
        return {};
    }
    return get_from_this_lua_state(command);
}
