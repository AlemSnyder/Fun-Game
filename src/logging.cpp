#include "logging.hpp"

#include "config.h"
#include "util/files.hpp"

#include <quill/handlers/JsonFileHandler.h>
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

quill::Logger* opengl_logger;   // for glfw, glew etc
quill::Logger* terrain_logger;  // for terrain, chunk, tile class
quill::Logger* game_map_logger; // for terrain generation
quill::Logger* voxel_logger;    // for voxel logic like mesh creation
quill::Logger* file_io_logger;  // for file io
quill::Logger* lua_logger;

const static std::filesystem::path LOG_FILE = log_dir() / "app.log";

void
init(bool console, quill::LogLevel log_level, bool structured) {
    _LOG_LEVEL = log_level;

    // Create the logs directory
    if (!std::filesystem::is_directory(log_dir()))
        std::filesystem::create_directory(log_dir());

    // Create our config object
    quill::Config cfg;

    // Set main logger name
    cfg.default_logger_name = "main";

    // Initialize print handler
    if (console) {
        quill::ConsoleColours colors;
        colors.set_colour(LogLevel::TraceL3, cc::black);
        colors.set_colour(LogLevel::TraceL2, cc::black);
        colors.set_colour(LogLevel::TraceL1, cc::black);
        colors.set_colour(LogLevel::Debug, cc::white);
        colors.set_colour(LogLevel::Info, cc::green);

#ifdef _WIN32
        colors.set_colour(LogLevel::Warning, cc::yellow | cc::bold);
        colors.set_colour(LogLevel::Error, cc::red | cc::bold);
        colors.set_colour(LogLevel::Critical, cc::bold | cc::white | cc::on_red);
#else
        colors.set_colour(LogLevel::Warning, cc::yellow + cc::bold);
        colors.set_colour(LogLevel::Error, cc::red + cc::bold);
        colors.set_colour(LogLevel::Critical, cc::bold + cc::white + cc::on_red);
#endif

        colors.set_colour(LogLevel::Backtrace, cc::magenta);

        auto stdout_handler = dynamic_cast<quill::ConsoleHandler*>(
            quill::stdout_handler("console", colors)
        );
        stdout_handler->set_pattern(
            LOGLINE_FORMAT,
            "%F %T.%Qms %z" // ISO 8601 but with space instead of T
        );
        stdout_handler->set_log_level(log_level);

        cfg.default_handlers.emplace_back(stdout_handler);
    }

    // Initialize file handler
    quill::Handler* file_handler;
#if DEBUG()
    (void)structured; // Keep the compiler from complaining

    // Rotate through file handlers to save space
    file_handler = quill::rotating_file_handler(
        LOG_FILE,
        "a",             // append
        1024 * 1024 / 2, // 512 KB
        5                // 5 backups
    );
#else
    if (structured) {
        file_handler = quill::create_handler<quill::JsonFileHandler>(
            (LOG_FILE + ".json").string(),
            "w",                            // Create a new file for every run
            quill::FilenameAppend::DateTime // Append datatime to make file unique
        );
    } else {
        // Create a new log file for each run
        file_handler = quill::file_handler(
            LOG_FILE,
            "w",                            // Create a new file for every run
            quill::FilenameAppend::DateTime // Append datatime to make file unique
        );
    }
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

    opengl_logger = get_logger("shaders");
    terrain_logger = get_logger("terrain");
    game_map_logger = get_logger("game_map");
    voxel_logger = get_logger("voxel");
    file_io_logger = get_logger("file_io");
    lua_logger = get_logger("lua");

    // Start the logging backend thread
    quill::start();

    LOG_INFO(main_logger, "Logging initialized!");
}

} // namespace logging
