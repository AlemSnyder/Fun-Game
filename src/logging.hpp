#pragma once

#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/sinks/FileSink.h>

#include "util/files.hpp"

#include <config.h>

#include <string>

namespace logging {

#if DEBUG()
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Debug;
#else
static constexpr quill::LogLevel DEFAULT_LOG_LEVEL = quill::LogLevel::Info;
#endif

extern quill::LogLevel _LOG_LEVEL;

extern quill::Logger* main_logger;
extern quill::Logger* opengl_logger;   // for glfw, glew etc
extern quill::Logger* terrain_logger;  // for terrain, chunk, tile class
extern quill::Logger* game_map_logger; // for terrain generation
extern quill::Logger* voxel_logger;    // for voxel logic like mesh creation
extern quill::Logger* file_io_logger;  // for file io
extern quill::Logger* lua_logger;      // for lua logging

/*
inline quill::Logger*
get_logger() {
    return quill::Frontend::get_logger();
}

inline quill::Logger*
get_logger(std::string name, std::shared_ptr<quill::Handler> handler = nullptr) {
    // if handler is nullptr will default to file_handler because
    // logging.cpp 142 `cfg.default_handlers.emplace_back(file_handler);`
    auto all_loggers = quill::get_all_loggers();

    // Search for the logger
    auto logger_found = all_loggers.find(name);

    // Found it, return it
    if (logger_found != all_loggers.end())
        return logger_found->second;

    // Create a new logger
    quill::Logger* logger;
    if (handler) {
        logger = quill::Frontend::create_or_get_logger(name, std::move(handler));
    } else {
        logger = quill::Frontend::create_or_get_logger(name);
    }
    logger->set_log_level(_LOG_LEVEL);
    logger->init_backtrace(5, quill::LogLevel::Error);
    return logger;
}*/

inline void
set_thread_name(std::string name) {
    quill::detail::set_thread_name(name.c_str());
}

inline std::filesystem::path
log_dir() noexcept {
    return files::get_root_path() / "logs";
}

void init(
    bool console = true, quill::LogLevel log_level = DEFAULT_LOG_LEVEL,
    bool structured = false
);

} // namespace logging
