#include "sky_data.hpp"

#include "../logging.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace sky {

SkyData::SkyData(const std::vector<glm::vec3>& stars) : num_stars_(stars.size()) {
    assert(glGetError() == GL_NO_ERROR);

    // LOG_DEBUG(logging::opengl_logger, "color texture id before {}", stars_texture_);
    glGenTextures(1, &stars_texture_);
    assert(glGetError() == GL_NO_ERROR);
    // LOG_DEBUG(logging::opengl_logger, "color texture id after binding {}",
    // stars_texture_);
    glBindTexture(GL_TEXTURE_1D, stars_texture_);
    assert(glGetError() == GL_NO_ERROR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    assert(glGetError() == GL_NO_ERROR);
    // set the texture wrapping/filtering options (on the currently bound texture
    // object)
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    // load and generate the texture
    glTexImage1D(
        GL_TEXTURE_1D, 0, GL_RGB32F, stars.size(), 0, GL_RGB, GL_FLOAT, stars.data()
    );
    assert(glGetError() == GL_NO_ERROR);
    glGenerateMipmap(GL_TEXTURE_1D);
    // assert(glGetError() == GL_NO_ERROR);
}

} // namespace sky

} // namespace gui
