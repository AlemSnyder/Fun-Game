#include "logging.hpp"

#include "config.h"
#include "global_context.hpp"
#include "util/files.hpp"

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/Logger.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>
#include <quill/sinks/RotatingFileSink.h>

#include <filesystem>
#include <string>

#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)                \
    || defined(QUILL_NO_THREAD_NAME_SUPPORT)
static const std::string LOGLINE_FORMAT =
    "%(time) [%(thread_id:>6)] %(log_level:<10) [%(logger:<10)] "
    "[%(short_source_location:<18)] "
    "- %(message)";
#else
static const std::string LOGLINE_FORMAT =
    "%(time) [%(thread_id:>6):%(thread_name:<16)] %(log_level:<10) "
    "[%(logger:<10)] [%(short_source_location:<18)] - %(message)";

#endif

#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)                \
    || defined(QUILL_NO_THREAD_NAME_SUPPORT)
static const std::string LOGLINE_FORMAT_LUA =
    "%(time) [%(thread_id:>6)] %(log_level:<10) [%(logger:<10)] "
    "%(message)";
#else
static const std::string LOGLINE_FORMAT_LUA =
    "%(time) [%(thread_id:>6):%(thread_name:<16)] %(log_level:<10) "
    "[%(logger:<10)] %(message)";

#endif

namespace logging {

using namespace quill;
using cc = quill::ConsoleColours;

LogLevel _LOG_LEVEL;

quill::Logger* main_logger;        // for everything else
quill::Logger* opengl_logger;      // for glfw, glew etc
quill::Logger* terrain_logger;     // for terrain, chunk, tile class
quill::Logger* game_map_logger;    // for terrain generation
quill::Logger* voxel_logger;       // for voxel logic like mesh creation
quill::Logger* file_io_logger;     // for file io
quill::Logger* lua_logger;         // for lua in cpp
quill::Logger* lua_runtime_logger; // for lua in lua

const static std::filesystem::path LOG_FILE = files::get_log_path() / "app.log";

void
init(bool console, quill::LogLevel log_level) {
    _LOG_LEVEL = log_level;

    // Create the logs directory
    if (!std::filesystem::is_directory(files::get_log_path()))
        std::filesystem::create_directory(files::get_log_path());

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

        auto console_sink =
            quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console", colors);
    }

    // Start the logging backend thread
    quill::Backend::start();

// Initialize file sink
#if DEBUG()
    auto file_sink =
        quill::Frontend::create_or_get_sink<quill::RotatingFileSink>(LOG_FILE, []() {
            quill::RotatingFileSinkConfig cfg;
            cfg.set_open_mode('a');
            cfg.set_rotation_max_file_size(1024 * 1024 / 2); // 512 KB
            cfg.set_max_backup_files(5);                     // 5 backups
            cfg.set_overwrite_rolled_files(true);            // append
            return cfg;
        }());
#else
    auto file_sink =
        quill::Frontend::create_or_get_sink<quill::FileSink>(LOG_FILE, []() {
            quill::FileSinkConfig cfg;
            cfg.set_open_mode('w');
            return cfg;
        }());

#endif

    // create all loggers
    main_logger =
        quill::Frontend::create_or_get_logger("main", file_sink, LOGLINE_FORMAT);
    opengl_logger =
        quill::Frontend::create_or_get_logger("shaders", file_sink, LOGLINE_FORMAT);
    terrain_logger =
        quill::Frontend::create_or_get_logger("terrain", file_sink, LOGLINE_FORMAT);
    game_map_logger =
        quill::Frontend::create_or_get_logger("game_map", file_sink, LOGLINE_FORMAT);
    voxel_logger =
        quill::Frontend::create_or_get_logger("voxel", file_sink, LOGLINE_FORMAT);
    file_io_logger =
        quill::Frontend::create_or_get_logger("file_io", file_sink, LOGLINE_FORMAT);
    lua_logger =
        quill::Frontend::create_or_get_logger("lua", file_sink, LOGLINE_FORMAT);
    lua_runtime_logger = quill::Frontend::create_or_get_logger(
        "lua_runtime", file_sink, LOGLINE_FORMAT_LUA
    );

    main_logger->set_log_level(_LOG_LEVEL);
    opengl_logger->set_log_level(_LOG_LEVEL);
    terrain_logger->set_log_level(_LOG_LEVEL);
    game_map_logger->set_log_level(_LOG_LEVEL);
    voxel_logger->set_log_level(_LOG_LEVEL);
    file_io_logger->set_log_level(_LOG_LEVEL);
    lua_logger->set_log_level(_LOG_LEVEL);
    lua_runtime_logger->set_log_level(_LOG_LEVEL);

    // initialize backtrace on all loggers
    // is there a better way?
    main_logger->init_backtrace(5, quill::LogLevel::Error);
    opengl_logger->init_backtrace(5, quill::LogLevel::Error);
    terrain_logger->init_backtrace(5, quill::LogLevel::Error);
    game_map_logger->init_backtrace(5, quill::LogLevel::Error);
    voxel_logger->init_backtrace(5, quill::LogLevel::Error);
    file_io_logger->init_backtrace(5, quill::LogLevel::Error);
    lua_logger->init_backtrace(5, quill::LogLevel::Error);
    lua_runtime_logger->init_backtrace(5, quill::LogLevel::Error);

    LOG_INFO(main_logger, "Logging initialized!");
}

} // namespace logging
