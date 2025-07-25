#pragma once

#include "logging.hpp"
#include "util/files.hpp"

#include <fmt/core.h>
#include <sol/sol.hpp>

namespace lua_logging {

inline void
lua_log_backtrace(std::string file, int line, std::string message) {
    LOG_BACKTRACE(
        logging::lua_script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", file, line), message
    );
}

inline void
lua_log_info(std::string file, int line, std::string message) {
    LOG_INFO(
        logging::lua_script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", file, line), message
    );
}

inline void
lua_log_debug(std::string file, int line, std::string message) {
    LOG_DEBUG(
        logging::lua_script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", file, line), message
    );
}

inline void
lua_log_warning(std::string file, int line, std::string message) {
    LOG_WARNING(
        logging::lua_script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", file, line), message
    );
}

inline void
lua_log_error(std::string file, int line, std::string message) {
    LOG_ERROR(
        logging::lua_script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", file, line), message
    );
}

inline void
lua_log_critical(std::string file, int line, std::string message) {
    LOG_CRITICAL(
        logging::lua_script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", file, line), message
    );
}

inline void
setup_lua_logging(sol::state& lua) {
    lua.set_function("lua_log_backtrace", lua_log_backtrace);
    lua.set_function("lua_log_info", lua_log_info);
    lua.set_function("lua_log_debug", lua_log_debug);
    lua.set_function("lua_log_warning", lua_log_warning);
    lua.set_function("lua_log_error", lua_log_error);
    lua.set_function("lua_log_critical", lua_log_critical);

    std::filesystem::path logging_file_path =
        files::get_resources_path() / "lua" / "logging.lua";

    sol::table logging_library =
        lua.require_file("Logging", logging_file_path.string(), false);

    if (!logging_library.valid()) {
        LOG_WARNING(logging::main_logger, "Logging library did not load correctly.");
    }
}

} // namespace lua_logging