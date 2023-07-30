#include "shadow_map.hpp"

#include "../../handler.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <memory>
#include <stdexcept>

namespace gui {

namespace data_structures {

ShadowMap::ShadowMap(screen_size_t w, screen_size_t h) {
    shadow_width_ = w;
    shadow_height_ = h;

    glGenFramebuffers(1, &frame_buffer_id_);
    FrameBufferHandler::getInstance().bind_fbo(frame_buffer_id_);

    // Depth texture. Slower than a depth buffer, but you can sample it later in
    // your shader

    glGenTextures(1, &depth_texture_id_);
    glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_width_, shadow_height_, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture_id_, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::invalid_argument("Framebuffer is not ok");
    }
}

void
ShadowMap::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = light_direction;
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void
ShadowMap::set_depth_projection_matrix(glm::mat4 depth_projection_matrix) {
    depth_projection_matrix_ = depth_projection_matrix;
}

} // namespace render

} // namespace gui
