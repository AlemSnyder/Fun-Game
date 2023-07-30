// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file gui_logging.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines message_callback function
 *
 * @ingroup GUI
 *
 */

#pragma once

#include "../logging.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace gui {

// I decided not to do do this line by line because
// defines are hard, and I am lazy.
// (if were are going to do this line by line, then we should get the line in the log
// file) We are using a gl call back on all messages.
inline void GLAPIENTRY
message_callback(
    GLenum source, GLenum type, unsigned int id, GLenum severity,
    [[maybe_unused]] GLsizei length, const char* message,
    [[maybe_unused]] const void* userParam
) {
    std::string source_sting;
    std::string type_string;
    // clang-format off
    switch (source) {
        case GL_DEBUG_SOURCE_API:               source_sting = "API";               break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     source_sting = "Window System";     break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:   source_sting = "Shader Compiler";   break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:       source_sting = "Third Party";       break;
        case GL_DEBUG_SOURCE_APPLICATION:       source_sting = "Application";       break;
        case GL_DEBUG_SOURCE_OTHER:             source_sting = "Other";             break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               type_string = "Error";                  break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_string = "Deprecated Behaviour";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_string = "Undefined Behaviour";    break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_string = "Portability";            break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_string = "Performance";            break;
        case GL_DEBUG_TYPE_MARKER:              type_string = "Marker";                 break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          type_string = "Push Group";             break;
        case GL_DEBUG_TYPE_POP_GROUP:           type_string = "Pop Group";              break;
        case GL_DEBUG_TYPE_OTHER:               type_string = "Other";                  break;
    }
    //clang-format on
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            LOG_ERROR(
                logging::opengl_logger, "{}: {}, ({}): {}", type_string, source_sting, id,
                message
            );
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            LOG_WARNING(
                logging::opengl_logger, "{}: {}, ({}): {}", type_string, source_sting, id,
                message
            );
            break;

        case GL_DEBUG_SEVERITY_LOW:
            LOG_DEBUG(
                logging::opengl_logger, "{}: {}, ({}): {}", type_string, source_sting, id,
                message
            );
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            LOG_DEBUG(
                logging::opengl_logger, "{}: {}, ({}): {}", type_string, source_sting, id,
                message
            );
            break;
    }
}

} // namespace gui