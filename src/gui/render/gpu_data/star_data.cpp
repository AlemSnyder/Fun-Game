#include "star_data.hpp"

#include "logging.hpp"
#include "types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

StarShape::StarShape() :
    shape_buffer_(ArrayBuffer(std::vector<glm::vec2>({
        {0,  1 },
        {1,  0 },
        {-1, 0 },
        {0,  -1}
}))) {
    // four point making a diamond centered at 0,0
    // note the order maters as this uses GL_TRIANGLE_STRIP to render stars.
}

star_data
StarData::read_data_from_file(std::filesystem::path path) {
    Json::Value stars_json;
    auto stars_file = files::open_data_file(path);
    if (stars_file.has_value())
        stars_file.value() >> stars_json;
    else {
        LOG_WARNING(
            logging::file_io_logger, "Could not open sky data from file {}.", path
        );
        return {};
    }

    std::vector<glm::vec4> stars_positions;
    std::vector<GLfloat> star_age;

    for (const Json::Value& star : stars_json["stars"]["data"]) {
        float phi = glm::radians(star["phi"].asFloat());
        float theta = glm::radians(star["theta"].asFloat());

        glm::vec4 star_position(
            glm::cos(theta) * glm::sin(phi), glm::sin(theta) * glm::sin(phi),
            glm::cos(phi), star["brightness"].asFloat()
        );
        stars_positions.push_back(star_position);

        star_age.push_back(star["age"].asFloat());
    }

    return {stars_positions, star_age};
}

} // namespace gpu_data

} // namespace gui
