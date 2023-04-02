#include "../gui/meshloader.hpp"
#include "mesh.hpp"
// #include "static_mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace terrain {

class TerrainMesh : public MeshLoader::SingleComplexMesh {
 private:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    GLuint color_texture_;
    unsigned int num_vertices_;
    bool do_render_;

 public:
    inline TerrainMesh(const TerrainMesh& obj) :
        vertex_buffer_(obj.get_vertex_buffer()), color_buffer_(obj.get_color_buffer()),
        normal_buffer_(obj.get_normal_buffer()),
        element_buffer_(obj.get_element_buffer()),
        color_texture_(obj.get_color_texture()), num_vertices_(obj.get_num_vertices()),
        do_render_(obj.do_render()){};

    // copy operator
    inline TerrainMesh&
    operator=(const TerrainMesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        color_texture_ = obj.color_texture_;
        num_vertices_ = obj.num_vertices_;
        do_render_ = obj.do_render_;
        return *this;
    }

    inline TerrainMesh(){};
    TerrainMesh(const entity::Mesh& mesh);
    void init(const entity::Mesh& mesh);

    inline ~TerrainMesh() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
        // glDeleteTextures(1, &color_texture_);
    }

    [[nodiscard]] inline bool
    do_render() const noexcept override {
        return do_render_;
    }

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

    [[nodiscard]] inline unsigned int
    get_num_vertices() const noexcept override {
        return num_vertices_;
    }
};

} // namespace terrain
