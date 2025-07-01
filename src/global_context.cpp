#include "global_context.hpp"

#include "local_context.hpp"
#include "logging.hpp"

void
GlobalContext::run_opengl_queue() {
    std::lock_guard<std::mutex> lock(opengl_queue_mutex);
    if (!opengl_functions.empty()) [[unlikely]] {
#if 0
        LOG_DEBUG(
            logging::opengl_logger, "Running {} functions from opengl function pool.",
            opengl_functions.size()
        );
#endif

        while (!opengl_functions.empty()) {
            const auto& function = opengl_functions.front();
            function(); // run function
            opengl_functions.pop();
        }
        LOG_DEBUG_LIMIT(
            std::chrono::seconds{60}, logging::opengl_logger, "Completed opengl queue."
        );
    }
}

void
GlobalContext::require_lua_file(
    const std::string& key, const std::filesystem::path& path, bool create_global
) {
    // run this lambda on all threads in the thread pool
    // in reality should only require the file in one lua state, then copy the generated
    // byte code to all the others this will load onto all states including the one held
    // by the main thread. yes this should be made thread safe, but so should global
    // context It will be fixed before it becomes a problem, just not in this pr
    // (probably Lua-Setup)
    for (auto& [thread_id, local_context] : local_thread_contexts_) {
        sol::state& lua = local_context.get_lua_state();
        lua.require_file(key, path.string(), create_global);
    };
}

GlobalContext::GlobalContext() :
    thread_pool_([] { quill::detail::set_thread_name("BS Thread"); }) {}

std::optional<sol::object>
GlobalContext::get_from_lua(const std::string& command) {
    LOG_BACKTRACE(logging::lua_logger, "Attempting to index {}.", command);

    std::stringstream command_stream(command);

    std::string key;

    std::getline(command_stream, key, '\\');

    auto raw_result = lua_.get<sol::optional<sol::object>>(key);

    if (!raw_result) {
        LOG_WARNING(logging::lua_logger, "{} not valid.", key);
        return {};
    }

    sol::table result;

    while (std::getline(command_stream, key, '\\')) {
        if (!raw_result->is<sol::table>()) {
            LOG_WARNING(logging::lua_logger, "{} not index of table.", key);
            return {};
        }
        result = raw_result.value();

        if (!result.valid()) {
            LOG_WARNING(logging::lua_logger, "Could not find {}.", key);
            return {};
        }

        if (result == sol::lua_nil) {
            LOG_WARNING(
                logging::lua_logger, "Attempting to index {}. nil value at {}.",
                command, key
            );
            return {};
        }

        if (!result.is<sol::table>()) {
            LOG_WARNING(
                logging::lua_logger, "Attempting to index {}. {} not index of table.",
                command, key
            );
            return {};
        }

        raw_result = result.get<sol::optional<sol::object>>(key);

        if (!raw_result) {
            LOG_WARNING(logging::lua_logger, "{} not valid.", key);
            return {};
        }
    }

    // a sol object
    return raw_result.value();
}

void GlobalContext::load_script_file(const std::filesystem::path& path) {
    auto result = lua_.safe_script_file(path);

    if (!result.valid()) {
        sol::error err = result; // who designed this?
        std::string what = err.what();
        LOG_ERROR(logging::lua_logger, "{}", what);
    }
}
