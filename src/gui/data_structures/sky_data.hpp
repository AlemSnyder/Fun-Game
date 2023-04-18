#include <json/json.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

#pragma once

namespace gui {

namespace sky {

class SkyData {
 private:
    GLuint star_positions_;
    GLuint age_buffer_;
    GLuint shape_buffer_;
    unsigned int num_stars_;

 public:
    /**
     * @brief Construct a new Sky Data object
     *
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     *
     * @param obj
     */
    SkyData(const SkyData& obj) = delete;

    /**
     * @brief The copy operator
     *
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     *
     * @param obj
     * @return SkyData&
     */
    SkyData& operator=(const SkyData& obj) = delete;

    /**
     * @brief Construct a new Sky Data object, default constructor
     *
     */
    inline SkyData(){};

    SkyData(std::filesystem::path path);

    GLuint
    get_star_positions() const {
        return star_positions_;
    }

    GLuint
    get_star_age_() const {
        return age_buffer_;
    }

    GLuint
    get_star_shape() const {
        return shape_buffer_;
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
