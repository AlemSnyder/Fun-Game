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
 * @ingroup GUI  array_buffer
 *
 */

#pragma once

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/vertex_array_object.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#include <glaze/glaze.hpp>
#pragma clang diagnostic pop

#include <filesystem>
#include <vector>

namespace gui {

namespace gpu_data {

struct planet_data_t {
    float radius;
    float brightness;
    float phase;
};

struct planets_data_t {
    float brightness_scale;
    std::vector<planet_data_t> data;
};

struct star_data_t {
    float theta;
    float phi;
    float brightness;
    float age;
};

struct stars_data_t {
    float brightness_scale;
    std::vector<star_data_t> data;
};

struct night_data_t {
    stars_data_t stars;
    planets_data_t planets;
};

/**
 * @brief Hold data for shape of star. This is used for the sun, and inherited for the
 * stars.
 *
 * GPUData is an interface used to render objects.
 */
class StarShape : public virtual GPUData {
 protected:
    VertexArrayObject vertex_array_object_;

    VertexBufferObject<glm::vec2> shape_buffer_; // id of vertex buffer of star shape

 public:
    StarShape(const StarShape& obj) = delete;
    StarShape& operator=(const StarShape& obj) = delete;

    /**
     * @brief Only constructor is default constructor.
     */
    StarShape(bool b = true);

    void initialize();

    virtual void attach_all();

    inline virtual ~StarShape() {}

    /**
     * @brief Get the star shape buffer.
     *
     * @return const VertexBufferObject<glm::vec2>& shape_buffer_
     */
    [[nodiscard]] inline const VertexBufferObject<glm::vec2>&
    get_star_shape() const {
        return shape_buffer_;
    }

    /**
     * @brief Bind star shape to curent program.
     */
    inline void
    bind() const override {
        vertex_array_object_.bind();
    }

    /**
     * @brief Release star shape from curent program.
     */
    inline void
    release() const override {
        vertex_array_object_.release();
    }

    /**
     * @brief If the stars should be rendered
     *
     * @return bool true
     */
    inline bool
    do_render() const override {
        return true;
    }

    /**
     * @brief The number of vertices a star has
     *
     * @return uint32_t 4
     */
    inline uint32_t
    get_num_vertices() const override {
        return 4;
    }
};

/**
 * @brief The data read from the stars json file.
 */
struct star_data {
    std::vector<glm::vec4> star_position;
    std::vector<GLfloat> star_age;
};

/**
 * @brief Contains a shadow depth buffer that can be rendered to.
 *
 * @details ShadowMap holds the depth texture. When added to a scene object
 * shadows are cast to this depth texture, and used when rendering the scene.
 *
 * StarsShape holds the buffer for the shape of each star.
 * GPUDataInstanced is an interface used to render instanced objects.
 */
class StarData : public StarShape, public virtual GPUDataInstanced {
 private:
    VertexBufferObject<glm::vec4>
        star_positions_;                     // id of vertex buffer for star positions
    VertexBufferObject<GLfloat> age_buffer_; // id of vertex buffer for star age
    size_t num_stars_;                       // number of stars to draw

 public:
    /**
     * @brief Delete copy constructor.
     */
    StarData(const StarData& obj) = delete;

    /**
     * @brief Delete copy operator.
     */
    StarData& operator=(const StarData& obj) = delete;

    /**
     * @brief Read star data from json file.
     */
    static star_data read_data_from_file(std::filesystem::path path);

    /**
     * @brief StarData Constructor from path to stars json file
     *
     * @param stars_data data Data read from stars json file or otherwise.
     */
    inline StarData(const star_data data) :
        StarShape(false), star_positions_(data.star_position, 1),
        age_buffer_(data.star_age, 1), num_stars_(data.star_age.size()) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { initialize(); });
    }

    /**
     * @brief StarData Constructor from path to stars json file
     *
     * @param std::filesystem::path path path to stars json file
     */
    StarData(std::filesystem::path path) : StarData(read_data_from_file(path)) {}

    virtual void attach_all();

    /**
     * @brief Get the star position buffer.
     *
     * @return const VertexBufferObject<glm::vec4>& star_position_
     */
    [[nodiscard]] inline const VertexBufferObject<glm::vec4>&
    get_star_positions() const {
        return star_positions_;
    }

    /**
     * @brief Get the star age buffer.
     *
     * @return const VertexBufferObject<GLfloat>& star_age_
     */
    [[nodiscard]] inline const VertexBufferObject<GLfloat>&
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

    /**
     * @brief Get the number of stars
     *
     * @details Same as get_num_stars. Exists for virtual function.
     *
     * @return size_t number of stars
     */
    inline uint32_t
    get_num_models() const override {
        return get_num_stars();
    }
};

} // namespace gpu_data

} // namespace gui
