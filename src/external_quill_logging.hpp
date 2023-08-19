// modified from quill/include/quill/detail/LogMacros.h

/**
 * Copyright(c) 2020-present, Odysseas Georgoudis & quill contributors.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

#pragma once

#include "logging.hpp"

#include <quill/Quill.h>

#define EXTERN_QUILL_LOGGER_CALL(                                                      \
    function_name, line, file, likelyhood, logger, log_statement_level, fmt, ...       \
)                                                                                      \
    do {                                                                               \
        struct {                                                                       \
            quill::MacroMetadata                                                       \
            operator()() const noexcept {                                              \
                return quill::MacroMetadata{                                           \
                    QUILL_STRINGIFY(line),                                             \
                    file,                                                              \
                    function_name,                                                     \
                    fmt,                                                               \
                    log_statement_level,                                               \
                    quill::MacroMetadata::Event::Log,                                  \
                    quill::detail::detect_structured_log_template(fmt)};               \
            }                                                                          \
        } anonymous_log_message_info;                                                  \
                                                                                       \
        if (likelyhood(logger->template should_log<log_statement_level>())) {          \
            logger->template log<decltype(anonymous_log_message_info)>(                \
                FMT_STRING(fmt), ##__VA_ARGS__                                         \
            );                                                                         \
        }                                                                              \
    } while (0)

struct external_anonymous_log_message_info {
    const char* file;
    const char* function_name;
    const char* fmt;
    uint line;
    quill::LogLevel log_statement_level;

    constexpr quill::MacroMetadata

    operator()() const noexcept {
        return quill::MacroMetadata{
            QUILL_STRINGIFY(line),
            file,
            function_name,
            fmt,
            log_statement_level,
            quill::MacroMetadata::Event::Log,
            quill::detail::detect_structured_log_template(fmt)};
    }
};

class LuaLogger {
 private:
    quill::Logger* logger_;

    inline LuaLogger() : logger_(logging::lua_logger) {}

    void
    extern_log(
        const std::string lua_function, const std::string lua_file,
        const size_t lua_line, const std::string lua_log_msg, const size_t lua_log_level

    ) {

        const std::string file_line = fmt::fmt("{}:{}", lua_file, lua_line);

        static const char* function_name = lua_function.c_str();

        struct {
            quill::MacroMetadata
            operator()() const noexcept {
                return quill::MacroMetadata{
                    lua_function.c_str(),
                    lua_file.c_str(),
                    file_line.c_str(),
                    function_name,
                    "{}",
                    quill::LogLevel::Dynamic,
                    quill::MacroMetadata::Event::Log,
                    false,
                    false};
            }
        } anonymous_log_message_info;

        if (logger->should_log(lua_log_level)) {
            logger->template log<decltype(anonymous_log_message_info)>(
                lua_log_level, QUILL_FMT_STRING("{}"), lua_log_msg
            );
        }
    }

 public:
    static inline LuaLogger&
    getInstance() {
        static LuaLogger lua_logger;
        return lua_logger;
    }

    LuaLogger(LuaLogger&&) = delete;
    LuaLogger(LuaLogger const&) = delete;

    void operator=(LuaLogger&&) = delete;
    void operator=(LuaLogger const&) = delete;

    inline void
    log_trace_l3(
        uint line, const char* file, const char* function_name, const char* log
    ) {
        // extern_log(
        //     line, file, function_name, quill::LogLevel::Trace3, logger_, "{}", log
        //);
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_trace_l2(
        uint line, const char* file, const char* function_name, const char* log
    ) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_trace_l1(
        uint line, const char* file, const char* function_name, const char* log
    ) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_debug(uint line, const char* file, const char* function_name, const char* log) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_info(uint line, const char* file, const char* function_name, const char* log) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_warning(
        uint line, const char* file, const char* function_name, const char* log
    ) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_error(uint line, const char* file, const char* function_name, const char* log) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }

    inline void
    log_critical(
        uint line, const char* file, const char* function_name, const char* log
    ) {
        EXTERN_QUILL_LOGGER_CALL(
            function_name, line, file, QUILL_UNLIKELY, logger_,
            quill::LogLevel::TraceL3, "{}", log
        );
    }
};
