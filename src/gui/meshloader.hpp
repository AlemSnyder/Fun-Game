#pragma once

// Include GLEW
#include <GL/glew.h>

namespace MeshLoader {

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
    virtual unsigned int get_num_vertices() const = 0;
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
    virtual unsigned int get_num_vertices() const = 0;
    virtual unsigned int get_num_models() const = 0;
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

} // namespace MeshLoader
