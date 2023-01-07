#include "sky_data.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace sky {

SkyData::SkyData(const std::vector<glm::vec3>& stars) : num_stars_(stars.size()) {
    glGenTextures(1, &stars_texture_);
    // stars_texture_);
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
        GL_TEXTURE_1D, 0, GL_RGB32F, stars.size(), 0, GL_RGB, GL_FLOAT, stars.data()
    );
    glGenerateMipmap(GL_TEXTURE_1D);
}

} // namespace sky

} // namespace gui
