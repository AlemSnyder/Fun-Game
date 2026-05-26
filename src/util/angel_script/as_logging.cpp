#include "error_checks.hpp"
#include "local_context.hpp"
#include "logging.hpp"

#include <fmt/core.h>

#include <string>

namespace util {
namespace scripting {

namespace {

struct script_location_t {
    std::string file;
    int line;
    int column;
};

script_location_t
get_script_location() {
    script_location_t location_data;
    AngelScript::asIScriptContext* context = AngelScript::asGetActiveContext();
    if (context == nullptr) {
        location_data.file = "Invoked from c++";
        location_data.line = -1;
    } else {
        char* section_name;
        location_data.line = context->GetLineNumber(
            0, &location_data.column, const_cast<const char**>(&section_name)
        );
        if (section_name == nullptr) {
            location_data.file = "Unknown";
        } else {
            location_data.file = std::string(section_name);
        }
    }
    return location_data;
}

/**
 * @brief Backtrace Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void
as_log_backtrace(std::string message) {
    script_location_t location = get_script_location();
    LOG_BACKTRACE(
        logging::script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", location.file, location.line), message
    );
}

/**
 * @brief Info Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void
as_log_info(std::string message) {
    script_location_t location = get_script_location();
    LOG_INFO(
        logging::script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", location.file, location.line), message
    );
}

/**
 * @brief Debug Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void
as_log_debug(std::string message) {
    script_location_t location = get_script_location();
    LOG_DEBUG(
        logging::script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", location.file, location.line), message
    );
}

/**
 * @brief Warning Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void
as_log_warning(std::string message) {
    script_location_t location = get_script_location();
    LOG_WARNING(
        logging::script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", location.file, location.line), message
    );
}

/**
 * @brief Error Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void
as_log_error(std::string message) {
    script_location_t location = get_script_location();
    LOG_ERROR(
        logging::script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", location.file, location.line), message
    );
}

/**
 * @brief Critical Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void
as_log_critical(std::string message) {
    script_location_t location = get_script_location();
    LOG_CRITICAL(
        logging::script_logger, "[{:<18}] - {}",
        fmtquill::format("{}:{}", location.file, location.line), message
    );
}

} // namespace

void
init_as_interface(AngelScript::asIScriptEngine* engine) {
    int r = engine->SetDefaultNamespace("LOGGING");
    if (util::scripting::check_SetDefaultNamespace(r) < 0) {
        return;
    }
    r = engine->RegisterGlobalFunction(
        "void LOG_BACKTRACE(string)", AngelScript::asFUNCTION(as_log_backtrace),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterGlobalFunction(r) < 0) {
        return;
    }
    r = engine->RegisterGlobalFunction(
        "void LOG_INFO(string)", AngelScript::asFUNCTION(as_log_info),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterGlobalFunction(r) < 0) {
        return;
    }
    r = engine->RegisterGlobalFunction(
        "void LOG_DEBUG(string)", AngelScript::asFUNCTION(as_log_debug),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterGlobalFunction(r) < 0) {
        return;
    }
    r = engine->RegisterGlobalFunction(
        "void LOG_WARNING(string)", AngelScript::asFUNCTION(as_log_warning),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterGlobalFunction(r) < 0) {
        return;
    }
    r = engine->RegisterGlobalFunction(
        "void LOG_ERROR(string)", AngelScript::asFUNCTION(as_log_error),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterGlobalFunction(r) < 0) {
        return;
    }
    r = engine->RegisterGlobalFunction(
        "void LOG_CRITICAL(string)", AngelScript::asFUNCTION(as_log_critical),
        AngelScript::asCALL_CDECL
    );
    if (util::scripting::check_RegisterGlobalFunction(r) < 0) {
        return;
    }
}

} // namespace scripting

} // namespace util
