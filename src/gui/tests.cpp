#include "gui/handler.hpp"
#include "gui/render/gpu_data/multi_buffer.hpp"
#include "gui/ui/opengl_setup.hpp"
#include "logging.hpp"
#include "types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <optional>
#include <string>

namespace gui {

int
opengl_tests() {
    // add window width and height?

    screen_size_t window_width = 800;
    screen_size_t window_height = 800;
    // init graphics

    std::optional<GLFWwindow*> opt_window =
        gui::setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    gui::setup_opengl_logging();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    gui::VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);

    GlobalContext& context = GlobalContext::instance();

    LOG_DEBUG(logging::opengl_logger, "READ WRITE TEST");
    {
        std::vector<int> data = {1, 2, 3, 4, 5, 6};
        gpu_data::VertexBufferObject<int> gpu_vector_data(data);

        context.run_opengl_queue();

        std::vector<int> read_data = gpu_vector_data.read();

        if (!(data == read_data)) {
            LOG_ERROR(logging::opengl_logger, "Read Write not equivalent.");

            LOG_DEBUG(logging::opengl_logger, "In: {}", data);

            LOG_DEBUG(logging::opengl_logger, "Out: {}", read_data);

            return 1;
        }
    }
    LOG_DEBUG(logging::opengl_logger, "UPDATE TEST");
    {
        std::vector<int> data = {1, 2, 3, 4, 5, 6};
        gpu_data::VertexBufferObject<int> gpu_vector_data;

        gpu_vector_data.update(data);

        context.run_opengl_queue();

        std::vector<int> read_data = gpu_vector_data.read();

        if (!(data == read_data)) {
            LOG_ERROR(logging::opengl_logger, "Read Write not equivalent.");

            LOG_DEBUG(logging::opengl_logger, "In: {}", data);

            LOG_DEBUG(logging::opengl_logger, "Out: {}", read_data);

            return 1;
        }
    }
    LOG_DEBUG(logging::opengl_logger, "INSERT TEST");
    {
        std::vector<int> data_1 = {1, 2, 3, 4, 5, 6};

        std::vector<int> data_2 = {1, 2, 3, 4, 5, 6};

        std::vector<int> expected_data({1, 2, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6});
        gpu_data::VertexBufferObject<int> gpu_vector_data(data_1);

        gpu_vector_data.insert(data_2, 2, 2);

        context.run_opengl_queue();

        std::vector<int> read_data = gpu_vector_data.read();

        if (!(read_data == expected_data)) {
            LOG_ERROR(logging::opengl_logger, "Read Write not equivalent.");

            LOG_DEBUG(logging::opengl_logger, "Expected: {}", expected_data);

            LOG_DEBUG(logging::opengl_logger, "Out: {}", read_data);

            return 1;
        }
    }

    LOG_DEBUG(logging::opengl_logger, "OVERWRITE TEST");
    {
        std::vector<int> data_1 = {1, 2, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6};

        std::vector<int> data_2 = {1, 2, 3, 4, 5, 6};

        std::vector<int> expected_data({1, 2, 1, 2, 3, 4, 1, 2, 3, 4, 5, 6});
        gpu_data::VertexBufferObject<int> gpu_vector_data(data_1);

        gpu_vector_data.insert(data_2, 6, 12);

        context.run_opengl_queue();

        std::vector<int> read_data = gpu_vector_data.read();

        if (!(read_data == expected_data)) {
            LOG_ERROR(logging::opengl_logger, "Read Write not equivalent.");

            LOG_DEBUG(logging::opengl_logger, "Expected: {}", expected_data);

            LOG_DEBUG(logging::opengl_logger, "Out: {}", read_data);

            return 1;
        }
    }

#if 1
    LOG_DEBUG(logging::opengl_logger, "OVERWRITE TEST");
    {
        std::vector<glm::vec2> data_1({
            {1, 2},
            {1, 2},
            {3, 4},
            {5, 6},
            {3, 4},
            {5, 6}
        });

        std::vector<glm::vec2> data_2({
            {7, 6},
            {5, 4},
            {3, 2}
        });

        std::vector<glm::vec2> expected_data({
            {1, 2},
            {7, 6},
            {5, 4},
            {3, 2},
            {5, 6}
        });
        gpu_data::VertexBufferObject<glm::vec2> gpu_vector_data(data_1);

        gpu_vector_data.insert(data_2, 1, 5);

        context.run_opengl_queue();

        std::vector<glm::vec2> read_data = gpu_vector_data.read();

        if (!(read_data == expected_data)) {
            LOG_ERROR(logging::opengl_logger, "Read Write not equivalent.");

            std::vector<std::string> expected_data_s;
            for (const auto& vec : expected_data) {
                expected_data_s.push_back(glm::to_string(vec));
            }
            std::vector<std::string> read_data_s;
            for (const auto& vec : read_data) {
                read_data_s.push_back(glm::to_string(vec));
            }

            LOG_DEBUG(logging::opengl_logger, "Expected: {}", expected_data_s);

            LOG_DEBUG(logging::opengl_logger, "Out:      {}", read_data_s);

            return 1;
        }
    }
#endif

    glDeleteVertexArrays(1, &VertexArrayID);

    return 0;
}

} // namespace gui
