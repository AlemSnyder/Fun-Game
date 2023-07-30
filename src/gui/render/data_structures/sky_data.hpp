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

class SkyData {
 private:
    GLuint star_positions_;
    GLuint age_buffer_;
    GLuint shape_buffer_;
    size_t num_stars_;

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
     * @return unsigned int number of stars
     */
    [[nodiscard]] inline size_t
    get_num_stars() const {
        return num_stars_;
    }
};

} // namespace data_structures

} // namespace gui
