#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <json/json.h>

#include <filesystem>

#pragma once

namespace gui {

namespace sky {

class SkyData {
 private:
    GLuint stars_texture_;
    unsigned int num_stars_;

 public:
    /**
     * @brief Construct a new Sky Data object
     * 
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     * 
     * @param obj 
     */
    inline SkyData(const SkyData& obj) {
        stars_texture_ = obj.stars_texture_;
        num_stars_ = obj.num_stars_;
    };

    /**
     * @brief The copy operator
     * 
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     * 
     * @param obj 
     * @return SkyData& 
     */
    inline SkyData&
    operator=(const SkyData& obj) {
        stars_texture_ = obj.stars_texture_;
        num_stars_ = obj.num_stars_;
        return *this;
    }

    /**
     * @brief Construct a new Sky Data object, default constructor
     * 
     */
    inline SkyData(){};
    /**
     * @brief Construct a new Sky Data object
     * 
     * @param stars vector of theta, phi, brightness that describes stars
     */
    //SkyData(const std::vector<glm::vec3>& stars);

    SkyData(std::filesystem::path path);

    /**
     * @brief Destroy the Sky Data object
     * 
     */
    inline ~SkyData() { 
        glDeleteTextures(1, &stars_texture_);
    }

    /**
     * @brief Get the stars texture id
     * 
     * @return GLuint id of star texture on gpu
     */
    inline GLuint
    get_stars_texture() const {
        return stars_texture_;
    }
    /**
     * @brief Get the number of stars
     * 
     * @return unsigned int number of stars
     */
    inline unsigned int
    get_num_stars() const {
        return num_stars_;
    }
};

} // namespace sky

} // namespace gui
