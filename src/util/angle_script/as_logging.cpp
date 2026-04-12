#include "local_context.hpp"
#include "logging.hpp"

#include <fmt/core.h>

#include <string>

namespace as_logging {

inline void
as_log_backtrace(std::string message) {
    // get engine from local context
    // auto& localcontext = LocalContext::instance();
    // auto engine = localcontext.as_engine();

    std::string file("");
    int line = 5;
    LOG_BACKTRACE(
        logging::main_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
as_log_info(std::string message) {
    std::string file("");
    int line = 5;
    LOG_INFO(
        logging::main_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
as_log_debug(std::string message) {
    std::string file("");
    int line = 5;
    LOG_DEBUG(
        logging::main_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
as_log_warning(std::string message) {
    std::string file("");
    int line = 5;
    LOG_WARNING(
        logging::main_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
as_log_error(std::string message) {
    std::string file("");
    int line = 5;
    LOG_ERROR(
        logging::main_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

inline void
as_log_critical(std::string message) {
    std::string file("");
    int line = 5;
    LOG_CRITICAL(
        logging::main_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", file, line),
        message
    );
}

} // namespace as_logging
