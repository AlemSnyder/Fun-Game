#include "local_context.hpp"
#include "logging.hpp"

#include <fmt/core.h>

#include <string>

namespace as_logging {

struct script_location {
    std::string file;
    int line;
    int column;
};

script_location get_script_location() {
    script_location location_data;
    asIScriptContext* context = asGetActiveContext();
    if (context == nullptr) {
        location_data.file = "Invoked from c++";
        location_data.line = -1;
    } else {
        char* section_name;
        location_data.line = context->GetLineNumber(0, &location_data.column, const_cast<const char**>(&section_name));
        if (section_name == nullptr){
            location_data.file = "Unknown";
        } else {
            location_data.file = std::string(section_name);
        }
    }
    return location_data;
}

void
as_log_backtrace(std::string message) {
    script_location location = get_script_location();
    LOG_BACKTRACE(
        logging::lua_script_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", location.file, location.line),
        message
    );
}

void
as_log_info(std::string message) {
    script_location location = get_script_location();
    LOG_INFO(
        logging::lua_script_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", location.file, location.line),
        message
    );
}

void
as_log_debug(std::string message) {    LocalContext& local_context = LocalContext::instance();
    script_location location = get_script_location();
    LOG_DEBUG(
        logging::lua_script_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", location.file, location.line),
        message
    );
}

void
as_log_warning(std::string message) {
    script_location location = get_script_location();
    LOG_WARNING(
        logging::lua_script_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", location.file, location.line),
        message
    );
}

void
as_log_error(std::string message) {
    script_location location = get_script_location();
    LOG_ERROR(
        logging::lua_script_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", location.file, location.line),
        message
    );
}

void
as_log_critical(std::string message) {
    script_location location = get_script_location();
    LOG_CRITICAL(
        logging::lua_script_logger, "[{:<18}] - {}", fmtquill::format("{}:{}", location.file, location.line),
        message
    );
}
void
init_as_interface(asIScriptEngine* engine) {
    int r = engine->SetDefaultNamespace("LOGGING");
    assert(r >= 0);

    // Registering 
    r = engine->RegisterGlobalFunction("void LOG_BACKTRACE(string)", asFUNCTION(as_log_backtrace), asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("void LOG_INFO(string)", asFUNCTION(as_log_info), asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("void LOG_DEBUG(string)", asFUNCTION(as_log_debug), asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("void LOG_WARNING(string)", asFUNCTION(as_log_warning), asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("void LOG_ERROR(string)", asFUNCTION(as_log_error), asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterGlobalFunction("void LOG_CRITICAL(string)", asFUNCTION(as_log_critical), asCALL_CDECL);
    assert(r >= 0);

}

} // namespace as_logging
