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
 * @brief Defines StarData class
 *
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

#pragma once

#include "../graphics_data/array_buffer.hpp"
#include "../graphics_data/gpu_data.hpp"

#include <json/json.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

namespace gui {

namespace data_structures {

class StarShape : public virtual GPUData {
 protected:
    ArrayBuffer shape_buffer_; // id of vertex buffer of star shape

 public:
    StarShape(const StarShape& obj) = delete;
    StarShape& operator=(const StarShape& obj) = delete;

    StarShape();

    [[nodiscard]] inline const ArrayBuffer
    get_star_shape() const {
        return shape_buffer_;
    }

    inline virtual void
    bind() const {
        shape_buffer_.bind(0, 0);
    }

    inline virtual void
    release() const {
        glDisableVertexAttribArray(0);
    }

    inline virtual bool
    do_render() const {
        return true;
    }

    inline virtual uint32_t
    get_num_vertices() const {
        return 4;
    }
};

struct star_data {
    std::vector<glm::vec4> star_position;
    std::vector<GLfloat> star_age;
};

/**
 * @brief Contains a shadow depth buffer that can be rendered to.
 *
 * @details ShadowMap holds the depth texture. When added to a scene object
 * shadows are cast to this depth texture, and used when rendering the scene.
 */
class StarData : public StarShape, public virtual GPUDataInstanced {
 private:
    ArrayBuffer star_positions_; // id of vertex buffer for star positions
    ArrayBuffer age_buffer_;     // id of vertex buffer for star age
    size_t num_stars_;           // number of stars to draw

 public:
    /**
     * @brief Construct a new Sky Data object
     *
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     *
     * @param obj
     */
    StarData(const StarData& obj) = delete;

    /**
     * @brief The copy operator
     *
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     *
     * @param obj
     * @return StarData&
     */
    StarData& operator=(const StarData& obj) = delete;

    static star_data read_data_from_file(std::filesystem::path path);

    inline StarData(const star_data data) :
        star_positions_(data.star_position), age_buffer_(data.star_age), num_stars_(data.star_age.size()) {}

    StarData(std::filesystem::path path) : StarData(read_data_from_file(path)) {}

    [[nodiscard]] inline const ArrayBuffer
    get_star_positions() const {
        return star_positions_;
    }

    [[nodiscard]] inline const ArrayBuffer
    get_star_age_() const {
        return age_buffer_;
    }

    /**
     * @brief Get the number of stars
     *
     * @return size_t number of stars
     */
    [[nodiscard]] inline uint32_t
    get_num_stars() const {
        return num_stars_;
    }

    inline virtual uint32_t
    get_num_models() const {
        return get_num_stars();
    }
};

} // namespace data_structures

} // namespace gui
