#include "star_data.hpp"

#include "../../../entity/mesh.hpp"
#include "../../../logging.hpp"
#include "../../../types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace data_structures {

SkyData::SkyData(std::filesystem::path path) {
    Json::Value stars_json;
    auto stars_file = files::open_data_file(path);
    if (stars_file.has_value())
        stars_file.value() >> stars_json;
    else {
        LOG_WARNING(
            logging::file_io_logger, "Could not open sky data from file {}.", path
        );
        return;
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

    num_stars_ = stars_positions.size();

    // A buffer for the vertex positions
    glGenBuffers(1, &star_positions_);
    glBindBuffer(GL_ARRAY_BUFFER, star_positions_);
    glBufferData(
        GL_ARRAY_BUFFER, stars_positions.size() * sizeof(glm::vec4),
        stars_positions.data(), GL_STATIC_DRAW
    );

    // A buffer for the colors
    glGenBuffers(1, &age_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, age_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, star_age.size() * sizeof(GLfloat), star_age.data(),
        GL_STATIC_DRAW
    );

    // four point making a diamond centered at 0,0
    // note the order maters as this uses GL_TRIANGLE_STRIP to render stars.
    std::vector<glm::vec2> star_shape({
        {0,  1 },
        {1,  0 },
        {-1, 0 },
        {0,  -1}
    });

    // Generate a buffer for the for corners of a "star"
    glGenBuffers(1, &shape_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, shape_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, star_shape.size() * sizeof(glm::vec2), star_shape.data(),
        GL_STATIC_DRAW
    );
}

} // namespace data_structures

} // namespace gui
