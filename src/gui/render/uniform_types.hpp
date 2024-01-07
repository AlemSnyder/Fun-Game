#pragma once

#include "../../types.hpp"
#include "../scene/controls.hpp"
#include "data_structures/shadow_map.hpp"
#include "graphics_shaders/uniform.hpp"

#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace render {

class LightEnvironment {
 private:
 public:
    virtual ~LightEnvironment(){};

    virtual glm::vec3 get_light_direction() const = 0;
    virtual glm::vec3 get_diffuse_light() const = 0;
    virtual glm::vec3 get_specular_light() const = 0;
};

class StarRotation {
 public:
    virtual ~StarRotation(){};
    virtual glm::mat4 get_sky_rotation() const = 0;
};

class LightDirection : public shader::Uniform {
 private:
    std::shared_ptr<LightEnvironment> lighting_;

 public:
    LightDirection(std::shared_ptr<LightEnvironment> lighting) :
        Uniform("light_direction"), lighting_(lighting) {}

    virtual ~LightDirection(){};

    virtual void
    bind() {
        const glm::vec3& light_direction = lighting_->get_light_direction();

        // set the light direction uniform
        glUniform3f(
            uniform_ID_, light_direction.x, light_direction.y, light_direction.z
        );
    }
};

class DiffuseLight : public shader::Uniform {
 private:
    std::shared_ptr<LightEnvironment> lighting_;

 public:
    DiffuseLight(std::shared_ptr<LightEnvironment> lighting) :
        Uniform("diffuse_light_color"), lighting_(lighting) {}

    virtual ~DiffuseLight(){};

    virtual void
    bind() {
        const glm::vec3 diffuse_light_color = lighting_->get_diffuse_light();

        glUniform3f(
            uniform_ID_, diffuse_light_color.r, diffuse_light_color.g,
            diffuse_light_color.b
        );
    }
};

class SpectralLight : public shader::Uniform {
 private:
    std::shared_ptr<render::LightEnvironment> lighting_;

 public:
    SpectralLight(std::shared_ptr<LightEnvironment> lighting) :
        Uniform("direct_light_color"), lighting_(lighting) {}

    virtual ~SpectralLight(){};

    inline virtual void
    bind() {
        const glm::vec3 sunlight_color = lighting_->get_specular_light();

        glUniform3f(uniform_ID_, sunlight_color.r, sunlight_color.g, sunlight_color.b);
    }
};

class MatrixViewProjection : public shader::Uniform {
 public:
    MatrixViewProjection() : Uniform("MVP") {}

    virtual ~MatrixViewProjection(){};

    inline virtual void
    bind() {
        const glm::mat4 view_matrix = controls::get_view_matrix();
        const glm::mat4 projection_matrix = controls::get_projection_matrix();

        glm::mat4 MVP = projection_matrix * view_matrix;

        glUniformMatrix4fv(uniform_ID_, 1, GL_FALSE, &MVP[0][0]);
    }
};

class ViewMatrix : public shader::Uniform {
 public:
    ViewMatrix() : Uniform("view_matrix") {}

    virtual ~ViewMatrix(){};

    inline virtual void
    bind() {
        const glm::mat4 view_matrix = controls::get_view_matrix();

        glUniformMatrix4fv(uniform_ID_, 1, GL_FALSE, &view_matrix[0][0]);
    }
};

class LightDepthProjection : public shader::Uniform {
 private:
    const data_structures::ShadowMap* shadow_map_;

 public:
    LightDepthProjection(const data_structures::ShadowMap* shadow_map) :
        Uniform("depth_MVP"), shadow_map_(shadow_map) {}

    virtual ~LightDepthProjection() {}

    inline virtual void
    bind() {
        const glm::mat4& depth_projection_matrix =
            shadow_map_->get_depth_projection_matrix();
        const glm::mat4& depth_view_matrix = shadow_map_->get_depth_view_matrix();

        // matrix to calculate the length of a light ray in model space
        glm::mat4 depthMVP = depth_projection_matrix * depth_view_matrix;

        // Shadow bias matrix of-sets the shadows
        // converts -1,1 to 0,1 to go from opengl render region to texture
        // clang-format off
        glm::mat4 bias_matrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
        // clang-format on

        glm::mat4 depth_bias_MVP = bias_matrix * depthMVP;

        glUniformMatrix4fv(uniform_ID_, 1, GL_FALSE, &depth_bias_MVP[0][0]);
    }
};

class TextureUniform : public shader::Uniform {
 private:
    uint8_t texture_location_;

 public:
    TextureUniform(std::string name, uint8_t texture_location) :
        Uniform(name), texture_location_(texture_location) {}

    inline virtual void
    bind() {
        glUniform1i(uniform_ID_, texture_location_);
    }
};

class MatrixViewInverseProjection : public shader::Uniform {
 public:
    MatrixViewInverseProjection() : Uniform("MVIP") {}

    virtual ~MatrixViewInverseProjection() {}

    inline virtual void
    bind() {
        // Compute the MVP matrix from keyboard and mouse input
        glm::mat4 projection_matrix = controls::get_projection_matrix();
        glm::mat4 view_matrix = controls::get_view_matrix();
        glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

        glm::mat4 sky_rotation = glm::inverse(MVP);

        glUniformMatrix4fv(uniform_ID_, 1, GL_FALSE, &sky_rotation[0][0]);
    }
};

class PixelProjection : public shader::Uniform {
 private:
    inline static screen_size_t width_;
    inline static screen_size_t height_;

 public:
    PixelProjection() : Uniform("pixel_projection") {}

    virtual ~PixelProjection() {}

    inline virtual void
    bind() {
        // Compute the MVP matrix from keyboard and mouse input
        // clang-format off
        glm::mat4 pixel_window = {
            1.0 / static_cast<float>(width_), 0, 0, 0,
            0, 1.0 / static_cast<float>(height_), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
        // clang-format on

        glUniformMatrix4fv(uniform_ID_, 1, GL_FALSE, &pixel_window[0][0]);
    }

    inline static void update(screen_size_t width, screen_size_t height) {
        width_ = width;
        height_ = height;
    }
};

class StarRotationUniform : public shader::Uniform {
 private:
    std::shared_ptr<render::StarRotation> rotation_;

 public:
    StarRotationUniform(std::shared_ptr<render::StarRotation> rotation) :
        Uniform("star_rotation"), rotation_(rotation) {}

    virtual ~StarRotationUniform() {}

    inline virtual void
    bind() {
        // Compute the MVP matrix from keyboard and mouse input
        glm::mat4 star_rotation = rotation_->get_sky_rotation();

        glUniformMatrix4fv(uniform_ID_, 1, GL_FALSE, &star_rotation[0][0]);
    }
};

} // namespace render

} // namespace gui
