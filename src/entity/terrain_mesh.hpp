#include "../gui/meshloader.hpp"
#include "mesh.hpp"

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
    inline TerrainMesh(const TerrainMesh& other) = delete;
    // copy operator
    inline TerrainMesh& operator=(const TerrainMesh& other) = delete;
    inline TerrainMesh& operator=(TerrainMesh&& other);

    inline TerrainMesh(){};
    TerrainMesh(const entity::Mesh& mesh);
    void update(const entity::Mesh& mesh);

    inline ~TerrainMesh() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
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
