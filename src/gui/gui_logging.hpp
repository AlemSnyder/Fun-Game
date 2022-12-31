#pragma once

#include "../logging.hpp"

#include <glm/glm.hpp>

#include <string>

namespace gui {

// I decided not to do do this line by line because
// defines are hard, and I am lazy.
// (if were are going to do this line by line, then we should get the line in the log
// file) We are using a gl call back on all messages.
void GLAPIENTRY
message_callback(
    GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
    const char* message, const void* userParam
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
                logging::opengl_logger, "{}: {}, ({}): {}", type, source_sting, id,
                message
            );
            break;

        case GL_DEBUG_SEVERITY_MEDIUM:
            LOG_WARNING(
                logging::opengl_logger, "{}: {}, ({}): {}", type, source_sting, id,
                message
            );
            break;

        case GL_DEBUG_SEVERITY_LOW:
            LOG_DEBUG(
                logging::opengl_logger, "{}: {}, ({}): {}", type, source_sting, id,
                message
            );
            break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
            LOG_DEBUG(
                logging::opengl_logger, "{}: {}, ({}): {}", type, source_sting, id,
                message
            );
            break;
    }
}

} // namespace gui