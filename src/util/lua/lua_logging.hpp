#include "logging.hpp"

#include <fmt/core.h>
#include <sol/sol.hpp>

namespace lua_logging {

inline void
lua_log_backtrace(std::string file, int line, std::string message) {
    LOG_BACKTRACE(
        logging::lua_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
lua_log_info(std::string file, int line, std::string message) {
    LOG_INFO(
        logging::lua_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
lua_log_debug(std::string file, int line, std::string message) {
    LOG_DEBUG(
        logging::lua_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
lua_log_warning(std::string file, int line, std::string message) {
    LOG_WARNING(
        logging::lua_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
lua_log_error(std::string file, int line, std::string message) {
    LOG_ERROR(
        logging::lua_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
lua_log_critical(std::string file, int line, std::string message) {
    LOG_CRITICAL(
        logging::lua_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

void
setup_lua_logging(sol::state& lua) {
    lua.set_function("lua_log_backtrace", lua_log_backtrace);
    lua.set_function("lua_log_info",      lua_log_info);
    lua.set_function("lua_log_debug",     lua_log_debug);
    lua.set_function("lua_log_warning",   lua_log_warning);
    lua.set_function("lua_log_error",     lua_log_error);
    lua.set_function("lua_log_critical",  lua_log_critical);
}

} // namespace lua_logging