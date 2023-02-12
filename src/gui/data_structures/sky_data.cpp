#include "sky_data.hpp"

#include "../../util/files.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

namespace gui {

namespace sky {

SkyData::SkyData(std::filesystem::path path) {
    Json::Value stars_json;
    std::ifstream stars_file = files::open_data_file(path);
    stars_file >> stars_json;
    std::vector<glm::vec4> stars;

    for (Json::Value::ArrayIndex i = 0; i < stars_json["stars"]["data"].size(); i++) {
        Json::Value star = stars_json["stars"]["data"][i];
        glm::vec4 star_vector(
            star["theta"].asFloat(),
            star["phi"].asFloat(),
            star["brightness"].asFloat(),
            star["age"].asFloat()
        );
        stars.push_back(star_vector);
    }

    num_stars_ = stars.size();

    glGenTextures(1, &stars_texture_);
    glBindTexture(GL_TEXTURE_1D, stars_texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // set the texture wrapping/filtering options (on the currently bound texture
    // object)
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    glTexImage1D(
        GL_TEXTURE_1D, 0, GL_RGBA32F, num_stars_, 0, GL_RGBA, GL_FLOAT, stars.data()
    );
    glGenerateMipmap(GL_TEXTURE_1D);
}

} // namespace sky

} // namespace gui
