#pragma once

#include <angelscript.h>

#include <string>

namespace as_logging {

void as_log_backtrace(std::string message);

void as_log_info(std::string message);

void as_log_debug(std::string message);

void as_log_warning(std::string message);

void as_log_error(std::string message);

void as_log_critical(std::string message);

void init_as_interface(asIScriptEngine* engine);

} // namespace as_logging
