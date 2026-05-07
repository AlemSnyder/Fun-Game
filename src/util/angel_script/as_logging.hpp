// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file as_logging.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines functions that initialize AngelScript logging
 *
 */

#pragma once

#include <angelscript.h>

#include <string>

namespace as_logging {

/**
 * @brief Backtrace Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void as_log_backtrace(std::string message);

/**
 * @brief Info Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void as_log_info(std::string message);

/**
 * @brief Debug Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void as_log_debug(std::string message);

/**
 * @brief Warning Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void as_log_warning(std::string message);

/**
 * @brief Error Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void as_log_error(std::string message);

/**
 * @brief Critical Log API
 *
 * @param std::string message message to be logged
 *
 * @details Given a message this will log it to the log file. This function will
 * determine the AngelScript line and file from the calling context.
 */
void as_log_critical(std::string message);

/**
 * @brief Initialize Logging interface on given engine
 *
 * @param asIScriptEngine* engine the engine logging will be initialized on.
 */
void init_as_interface(asIScriptEngine* engine);

} // namespace as_logging
