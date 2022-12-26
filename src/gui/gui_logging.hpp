#pragma once

#include "../logging.hpp"

#include <glm/glm.hpp>

#include <string>

namespace gui {

// I decided not to do do this line by line because
// defines are hare, and I am lazy.
void GLAPIENTRY
MessageCallback(
    GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
    const char* message, const void* userParam
) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::string source_sting;
    std::string type_string;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            source_sting = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_sting = "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_sting = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            source_sting = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            source_sting = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            source_sting = "Other";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_string = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_string = "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_string = "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_string = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_string = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            type_string = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_string = "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_string = "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_string = "Other";
            break;
    }

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