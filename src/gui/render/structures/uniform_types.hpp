#pragma once

#include "gui/render/gl_enums.hpp"
#include "gui/render/gpu_data/shadow_map.hpp"
#include "gui/render/graphics_shaders/uniform_executor.hpp"
#include "gui/scene/controls.hpp"
#include "logging.hpp"
#include "types.hpp"

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

class LightDirection : public shader::UniformExecuter {
 private:
    std::shared_ptr<LightEnvironment> lighting_;

 public:
    LightDirection(std::shared_ptr<LightEnvironment> lighting) :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_VEC3), lighting_(lighting) {}

    virtual ~LightDirection(){};

    virtual void
    bind(GLint uniform_ID) const override {
        const glm::vec3& light_direction = lighting_->get_light_direction();

        LOG_BACKTRACE(
            logging::opengl_logger,
            "Uniform {}, value: ({}, {}, {}), being initialized.", uniform_ID,
            light_direction.x, light_direction.y, light_direction.z
        );

        // set the light direction uniform
        glUniform3f(
            uniform_ID, light_direction.x, light_direction.y, light_direction.z
        );
    }
};

class DiffuseLight : public shader::UniformExecuter {
 private:
    std::shared_ptr<LightEnvironment> lighting_;

 public:
    DiffuseLight(std::shared_ptr<LightEnvironment> lighting) :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_VEC3), lighting_(lighting) {}

    virtual ~DiffuseLight(){};

    virtual void
    bind(GLint uniform_ID) const override {
        const glm::vec3 diffuse_light_color = lighting_->get_diffuse_light();

        LOG_BACKTRACE(
            logging::opengl_logger,
            "Uniform {}, value: ({}, {}, {}), being initialized.", uniform_ID,
            diffuse_light_color.r, diffuse_light_color.g, diffuse_light_color.b
        );

        glUniform3f(
            uniform_ID, diffuse_light_color.r, diffuse_light_color.g,
            diffuse_light_color.b
        );
    }
};

class SpectralLight : public shader::UniformExecuter {
 private:
    std::shared_ptr<render::LightEnvironment> lighting_;

 public:
    SpectralLight(std::shared_ptr<LightEnvironment> lighting) :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_VEC3), lighting_(lighting) {}

    virtual ~SpectralLight(){};

    inline virtual void
    bind(GLint uniform_ID) const override {
        const glm::vec3 sunlight_color = lighting_->get_specular_light();

        LOG_BACKTRACE(
            logging::opengl_logger,
            "Uniform {}, value: ({}, {}, {}), being initialized.", uniform_ID,
            sunlight_color.r, sunlight_color.g, sunlight_color.b
        );

        // here
        glUniform3f(uniform_ID, sunlight_color.r, sunlight_color.g, sunlight_color.b);
    }
};

class MatrixViewProjection : public shader::UniformExecuter {
 public:
    MatrixViewProjection() : UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4) {}

    virtual ~MatrixViewProjection(){};

    inline virtual void
    bind(GLint uniform_ID) const override {
        const glm::mat4 view_matrix = controls::get_view_matrix();
        const glm::mat4 projection_matrix = controls::get_projection_matrix();

        glm::mat4 MVP = projection_matrix * view_matrix;

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &MVP[0][0]);
    }
};

class ViewMatrix : public shader::UniformExecuter {
 public:
    ViewMatrix() : UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4) {}

    virtual ~ViewMatrix(){};

    inline virtual void
    bind(GLint uniform_ID) const override {
        const glm::mat4 view_matrix = controls::get_view_matrix();

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &view_matrix[0][0]);
    }
};

class LightDepthProjection : public shader::UniformExecuter {
 private:
    const gpu_data::ShadowMap* shadow_map_;

 public:
    LightDepthProjection(const gpu_data::ShadowMap* shadow_map) :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4), shadow_map_(shadow_map) {}

    virtual ~LightDepthProjection() {}

    inline virtual void
    bind(GLint uniform_ID) const override {
        const glm::mat4& depth_projection_matrix =
            shadow_map_->get_depth_projection_matrix();
        const glm::mat4& depth_view_matrix = shadow_map_->get_depth_view_matrix();

        // matrix to calculate the length of a light ray in model space
        glm::mat4 depthMVP = depth_projection_matrix * depth_view_matrix;

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &depthMVP[0][0]);
    }
};

class LightDepthTextureProjection : public shader::UniformExecuter {
 private:
    const gpu_data::ShadowMap* shadow_map_;

 public:
    LightDepthTextureProjection(const gpu_data::ShadowMap* shadow_map) :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4), shadow_map_(shadow_map) {}

    virtual ~LightDepthTextureProjection() {}

    inline virtual void
    bind(GLint uniform_ID) const override {
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

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &depth_bias_MVP[0][0]);
    }
};

class TextureUniform : public shader::UniformExecuter {
 private:
    uint8_t texture_location_;

 public:
    TextureUniform(gpu_data::GPUDataType texture_type, uint8_t texture_location) :
        UniformExecuter(texture_type), texture_location_(texture_location) {}

    inline virtual ~TextureUniform() {}

    inline virtual void
    bind(GLint uniform_ID) const override {
        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, value {} being initialized.",
            uniform_ID, texture_location_
        );

        glUniform1i(uniform_ID, texture_location_);
    }
};

class MatrixViewInverseProjection : public shader::UniformExecuter {
 public:
    MatrixViewInverseProjection() :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4) {}

    virtual ~MatrixViewInverseProjection() {}

    inline virtual void
    bind(GLint uniform_ID) const override {
        // Compute the MVP matrix from keyboard and mouse input
        glm::mat4 projection_matrix = controls::get_projection_matrix();
        glm::mat4 view_matrix = controls::get_view_matrix();
        glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

        glm::mat4 sky_rotation = glm::inverse(MVP);

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &sky_rotation[0][0]);
    }
};

class PixelProjection : public shader::UniformExecuter {
 private:
    inline static screen_size_t width_;
    inline static screen_size_t height_;

 public:
    PixelProjection() : UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4) {}

    virtual ~PixelProjection() {}

    inline virtual void
    bind(GLint uniform_ID) const override {
        // Compute the MVP matrix from keyboard and mouse input
        // clang-format off
        glm::mat4 pixel_window = {
            1.0 / static_cast<float>(width_), 0, 0, 0,
            0, 1.0 / static_cast<float>(height_), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
        // clang-format on

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &pixel_window[0][0]);
    }

    inline static void
    update(screen_size_t width, screen_size_t height) {
        width_ = width;
        height_ = height;
    }
};

class StarRotationUniform : public shader::UniformExecuter {
 private:
    std::shared_ptr<render::StarRotation> rotation_;

 public:
    StarRotationUniform(std::shared_ptr<render::StarRotation> rotation) :
        UniformExecuter(gpu_data::GPUDataType::FLOAT_MAT4), rotation_(rotation) {}

    virtual ~StarRotationUniform() {}

    inline virtual void
    bind(GLint uniform_ID) const override {
        // Compute the MVP matrix from keyboard and mouse input
        glm::mat4 star_rotation = rotation_->get_sky_rotation();

        LOG_BACKTRACE(
            logging::opengl_logger, "Uniform {}, being initialized.", uniform_ID
        );

        glUniformMatrix4fv(uniform_ID, 1, GL_FALSE, &star_rotation[0][0]);
    }
};

} // namespace render

} // namespace gui
