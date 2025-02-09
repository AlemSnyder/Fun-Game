#include "star_data.hpp"

#include "logging.hpp"
#include "types.hpp"
#include "util/files.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

StarShape::StarShape(bool b) :
    shape_buffer_{
        {0,  1 },
        {1,  0 },
        {-1, 0 },
        {0,  -1}
} {
    if (b) {
        // four point making a diamond centered at 0,0
        // note the order maters as this uses GL_TRIANGLE_STRIP to render stars.
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { initialize(); });
    }
}

void
StarShape::initialize() {
    vertex_array_object_.bind();
    attach_all();
    vertex_array_object_.release();
}

void
StarShape::attach_all() {
    shape_buffer_.attach_to_vertex_attribute(0);
}

star_data
StarData::read_data_from_file(std::filesystem::path path) {
    auto night_sky_file = files::open_data_file(path);

    auto night_data = files::read_json_from_file<night_data_t>(path);
    if (!night_data) {
        LOG_WARNING(
            logging::file_io_logger, "Could not open sky data from file {}.", path
        );
        return {};
    }

    std::vector<glm::vec4> stars_positions;
    std::vector<GLfloat> star_age;

    for (const auto& star : night_data->stars.data) {
        float phi = glm::radians(star.phi);
        float theta = glm::radians(star.theta);

        glm::vec4 star_position(
            glm::cos(theta) * glm::sin(phi), glm::sin(theta) * glm::sin(phi),
            glm::cos(phi), star.brightness
        );
        stars_positions.push_back(star_position);

        star_age.push_back(star.age);
    }

    return {stars_positions, star_age};
}

void
StarData::attach_all() {
    // look legacy
    //    StarShape::attach_all();
    star_positions_.attach_to_vertex_attribute(0);
    age_buffer_.attach_to_vertex_attribute(1);
    shape_buffer_.attach_to_vertex_attribute(2);
}

} // namespace gpu_data

} // namespace gui
