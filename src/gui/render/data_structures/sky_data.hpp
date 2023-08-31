// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file sky_data.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines SkyData class
 *
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

#pragma once

#include <json/json.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

namespace gui {

namespace data_structures {

/**
 * @brief Contains a shadow depth buffer that can be rendered to.
 *
 * @details ShadowMap holds the depth texture. When added to a scene object
 * shadows are cast to this depth texture, and used when rendering the scene.
 */
class SkyData {
 private:
    GLuint star_positions_; // id of vertex buffer for star positions
    GLuint age_buffer_;     // id of vertex buffer for star age
    GLuint shape_buffer_;   // id of vertex buffer of star shape
    size_t num_stars_;      // number of stars to draw

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

    ~SkyData() {
        glDeleteBuffers(1, &star_positions_);
        glDeleteBuffers(1, &age_buffer_);
        glDeleteBuffers(1, &shape_buffer_);
    }

    SkyData(std::filesystem::path path);

    [[nodiscard]] inline GLuint
    get_star_positions() const {
        return star_positions_;
    }

    [[nodiscard]] inline GLuint
    get_star_age_() const {
        return age_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_star_shape() const {
        return shape_buffer_;
    }

    /**
     * @brief Get the number of stars
     *
     * @return size_t number of stars
     */
    [[nodiscard]] inline size_t
    get_num_stars() const {
        return num_stars_;
    }
};

} // namespace data_structures

} // namespace gui
