#pragma once

#include <GL/glew.h>

// TODO
// so each virtuial calass coresponds to a fragment & vertex shader
// entities will use floats to store vertex positions, so they will need their
// own vertex shader, and therefore their own virtuial class.

// Fragment shaders can be reused though.

// data structures interface
// glew wrapper to send data to gpu
namespace MeshData {

/**
 * @brief virtual class for non-indexed mesh
 *
 * @details This will be used so that multiple different class types can all
 * be used by a generic mesh rendering function.
 */
class SingleMesh {
 public:
    virtual GLuint get_vertex_buffer() const = 0;
    virtual GLuint get_element_buffer() const = 0;
    virtual GLuint get_color_texture() const = 0;
    virtual unsigned int get_num_vertices() const = 0;
    virtual bool do_render() const = 0;
};

/**
 * @brief virtual class for indexed mesh
 *
 * @details This will be used so that multiple different class types can all
 * be used by a generic mesh rendering function.
 */
class MultiMesh {
 public:
    virtual GLuint get_vertex_buffer() const = 0;
    virtual GLuint get_element_buffer() const = 0;
    virtual GLuint get_model_transforms() const = 0;
    virtual GLuint get_color_texture() const = 0;
    virtual unsigned int get_num_vertices() const = 0;
    virtual unsigned int get_num_models() const = 0;
    virtual bool do_render() const = 0;
};

/**
 * @brief virtual class for non-indexed mesh
 *
 * @details This adds color, and normals to the mesh.
 */
class SingleComplexMesh : public SingleMesh {
 public:
    virtual GLuint get_normal_buffer() const = 0;
    virtual GLuint get_color_buffer() const = 0;
};

/**
 * @brief virtual class for indexed mesh
 *
 * @details This adds color, and normals to the mesh.
 */
class MultiComplexMesh : public MultiMesh {
 public:
    virtual GLuint get_normal_buffer() const = 0;
    virtual GLuint get_color_buffer() const = 0;
};

} // namespace MeshData
