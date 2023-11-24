#include "../../entity/mesh.hpp"
#include "../meshloader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace data_structures {

class Mesh : public MeshData::MultiComplexMesh {
 private:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    GLuint color_texture_;
    GLuint transforms_buffer_;
    uint32_t num_vertices_;
    uint32_t num_models_;
    bool do_render_;
    // end of override
    size_t bone_index_;

 public:
    inline Mesh(const Mesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        color_texture_ = obj.color_texture_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
        do_render_ = obj.do_render_;
    };

    // copy operator
    inline Mesh&
    operator=(const Mesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        color_texture_ = obj.color_texture_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
        do_render_ = obj.do_render_;
        return *this;
    }

    Mesh(
        const entity::Mesh& mesh,
        const std::vector<glm::mat4>& model_transforms = std::vector<glm::mat4>()
    );

    inline ~Mesh() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
        glDeleteTextures(1, &color_texture_);
        glDeleteBuffers(1, &transforms_buffer_);
    }

    // functions used by renderer are marked override
    [[nodiscard]] inline GLuint
    get_color_buffer() const noexcept override {
        return color_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_element_buffer() const noexcept override {
        return element_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_normal_buffer() const noexcept override {
        return normal_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_vertex_buffer() const noexcept override {
        return vertex_buffer_;
    }

    [[nodiscard]] inline GLuint
    get_color_texture() const noexcept override {
        return color_texture_;
    }

    [[nodiscard]] inline GLuint
    get_model_transforms() const noexcept override {
        return transforms_buffer_;
    }

    [[nodiscard]] inline uint32_t
    get_num_vertices() const noexcept override {
        return num_vertices_;
    }

    [[nodiscard]] inline uint32_t
    get_num_models() const noexcept override {
        return num_models_;
    }

    [[nodiscard]] inline bool
    do_render() const override {
        return do_render_;
    };

    // not used by renderer
    [[nodiscard]] inline size_t
    get_bone_index() const {
        return bone_index_;
    };

    void update_transforms(const std::vector<glm::mat4>& model_transforms);
};

} // namespace data_structures

} // namespace gui
