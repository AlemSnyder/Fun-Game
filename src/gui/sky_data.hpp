#include "../logging.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace sky {

class SkyData {
 private:
    GLuint stars_texture_;
    unsigned int num_stars_;

 public:
    inline SkyData(const SkyData& obj) {
        stars_texture_ = obj.stars_texture_;
        num_stars_ = obj.num_stars_;
    };

    // copy operator
    inline SkyData&
    operator=(const SkyData& obj) {
        stars_texture_ = obj.stars_texture_;
        num_stars_ = obj.num_stars_;
        return *this;
    }

    inline SkyData(){};

    SkyData(const std::vector<glm::vec3>& stars);

    inline ~SkyData() { glDeleteTextures(1, &stars_texture_); }

    inline GLuint
    get_stars_texture() const {
        return stars_texture_;
    }

    inline unsigned int
    get_num_stars() const {
        return num_stars_;
    }
};

} // namespace sky

} // namespace gui
