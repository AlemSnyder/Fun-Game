#include "logging.hpp"

#include "config.h"
#include "util/files.hpp"

#include <quill/Quill.h>

#include <filesystem>
#include <string>

#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)                \
    || defined(QUILL_NO_THREAD_NAME_SUPPORT)
static const std::string LOGLINE_FORMAT =
    "%(ascii_time) [%(thread:<6)] [%(fileline:<18)] %(level_name) [%(logger_name:<14)] "
    "- %(message)";
#else
static const std::string LOGLINE_FORMAT =
    "%(ascii_time) [%(thread:<16):%(thread_name:<16)] [%(fileline:<18)] %(level_name) "
    "[%(logger_name:<14)] - %(message)";

#endif

namespace logging {

using namespace quill;
using cc = quill::ConsoleColours;

LogLevel _LOG_LEVEL;

void
init(quill::LogLevel log_level, bool structured)
{
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
    quill::ConsoleColours colors;
    colors.set_colour(LogLevel::TraceL3, cc::black);
    colors.set_colour(LogLevel::TraceL2, cc::black);
    colors.set_colour(LogLevel::TraceL1, cc::black);
    colors.set_colour(LogLevel::Debug, cc::white);
    colors.set_colour(LogLevel::Info, cc::green);
    colors.set_colour(LogLevel::Warning, cc::yellow | cc::bold);
    colors.set_colour(LogLevel::Error, cc::red | cc::bold);
    colors.set_colour(LogLevel::Critical, cc::bold | cc::white | cc::on_red);
    colors.set_colour(LogLevel::Backtrace, cc::magenta);

    auto stdout_handler =
        dynamic_cast<quill::ConsoleHandler*>(quill::stdout_handler("console", colors));
    stdout_handler->set_pattern(
        LOGLINE_FORMAT,
        "%F %T.%Qms %z" // ISO 8601 but with space instead of T
    );
    stdout_handler->set_log_level(log_level);

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

    // Set backtrace and log level on the main logger
    quill::Logger* main_logger = quill::get_logger();
    main_logger->init_backtrace(5, quill::LogLevel::Error);

    // Start the logging backend thread
    quill::start();

    LOG_INFO(main_logger, "Logging initialized!");
}

} // namespace logging
