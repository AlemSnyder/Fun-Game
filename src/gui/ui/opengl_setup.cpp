#include "opengl_setup.hpp"

#include "../../logging.hpp"
#include "global_context.hpp"

#include <optional>

namespace gui {

std::optional<GLFWwindow*>
setup_opengl(screen_size_t window_width, screen_size_t window_height) {
    GlobalContext& context = GlobalContext::instance();
    assert(context.is_main_thread() && "OpenGL must be initialized on main thread.");
    glEnable(GL_MULTISAMPLE);

    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
        glfwTerminate();
        return nullptr;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // anti-aliasing of 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set Major
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // and Minor version
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    ); // To make MacOS happy; should not be needed
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    ); // somehow turning on core profiling

    // Tell GLFW we want logging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // Create window with graphics context
    GLFWwindow* window =
        glfwCreateWindow(window_width, window_height, "Fun-Game", nullptr, nullptr);
    if (window == nullptr)
        return nullptr;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLEW");
        glfwTerminate();
        return nullptr;
    }
    return window;
}

int
setup_opengl_logging() {
    GlobalContext& context = GlobalContext::instance();
    assert(context.is_main_thread() && "OpenGL must be initialized on main thread.");
    // initialize logging
    GLint context_flag;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flag);
    if (context_flag & GL_CONTEXT_FLAG_DEBUG_BIT) {
        LOG_INFO(logging::opengl_logger, "GLFW Logging with debug");
        try {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            // set gl message call back function
            glDebugMessageCallback(gui::message_callback, 0);
            glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE
            );
            LOG_INFO(logging::opengl_logger, "GLFW Logging initialized");
        } catch (...) {
            LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
            getchar();
            glfwTerminate();
            return -1;
        }
    } else {
        LOG_WARNING(logging::opengl_logger, "Will not log OpenGL.");
        return 1;
    }
    return 0;
}

} // namespace gui
