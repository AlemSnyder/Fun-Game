#include "logging.hpp"

#include "config.h"
#include "util/files.hpp"

#include <quill/Quill.h>

#include <filesystem>
#include <string>

static const std::string LOGLINE_FORMAT =
    "%(ascii_time) [%(thread)] [%(fileline:<18)] %(level_name) [%(logger_name:<14)] - "
    "%(message)";

namespace logging {

quill::LogLevel _LOG_LEVEL;

void
init(quill::LogLevel log_level) {
    _LOG_LEVEL = log_level;

    // Create the logs directory
    if (!std::filesystem::is_directory(log_dir())) {
        std::filesystem::create_directory(log_dir());
    }

    // Create our config object
    quill::Config cfg;

    // Set main logger name
    cfg.default_logger_name = "main";

    // Initialize print handler
    auto stdout_handler = dynamic_cast<quill::ConsoleHandler*>(quill::stdout_handler());
    stdout_handler->set_pattern(
        LOGLINE_FORMAT,
        "%F %T.%Qms %z" // ISO 8601 but with space instead of T
    );
    stdout_handler->set_log_level(log_level);
    stdout_handler->enable_console_colours();

    cfg.default_handlers.emplace_back(stdout_handler);

    // Initialize file handler
#if DEBUG()
    // Rotate through file handlers to save space
    quill::Handler* file_handler = quill::rotating_file_handler(
        log_dir() / "app.log", "a",
        1024 * 1024 / 2, // 512 KB
        5                // 5 backups
    );
#else
    // Create a new log file for each run
    quill::Handler* file_handler = quill::file_handler(
        log_dir() / "app.log", "w", quill::FilenameAppend::DateTime
    );
#endif

    file_handler->set_pattern(
        LOGLINE_FORMAT,
        "%FT%T.%Qms %z" // ISO 8601
    );
    file_handler->set_log_level(log_level);

    cfg.default_handlers.emplace_back(file_handler);

    // Send the config
    quill::configure(cfg);

    // Start the logging backend thread
    quill::start();

    LOG_INFO(quill::get_logger(), "Logging initialized!");
}

} // namespace logging
