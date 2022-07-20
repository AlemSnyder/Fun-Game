// Include GLEW
#include <GL/glew.h>

#ifndef __MESH_LOADER_HPP__
#define __MESH_LOADER_HPP__

namespace MeshLoader{

/**
 * @brief virtual class for non-indexed mesh
 * 
 * @details This will be used so that multiple different class types can all
 * be used by a generic mesh rendering function.
 */
class SingleMesh
{
public:
    virtual GLuint get_vertex_buffer() = 0;
    virtual GLuint get_element_buffer() = 0;
    virtual unsigned int get_num_vertices() = 0;
};

/**
 * @brief virtual class for indexed mesh
 * 
 * @details This will be used so that multiple different class types can all
 * be used by a generic mesh rendering function.
 */
class MultiMesh
{
public:
    virtual GLuint get_vertex_buffer() = 0;
    virtual GLuint get_element_buffer() = 0;
    virtual GLuint get_model_transforms() = 0;
    virtual unsigned int get_num_vertices() = 0;
    virtual unsigned int get_num_models() = 0;
};

/**
 * @brief virtual class for non-indexed mesh
 * 
 * @details This adds color, and normals to the mesh.
 */
class SingleComplexMesh : public SingleMesh{
public:
    virtual GLuint get_normal_buffer() = 0;
    virtual GLuint get_color_buffer() = 0;
};

/**
 * @brief virtual class for indexed mesh
 * 
 * @details This adds color, and normals to the mesh.
 */
class MultiComplexMesh : public MultiMesh {
public:
    virtual GLuint get_normal_buffer() = 0;
    virtual GLuint get_color_buffer() = 0;
};

} // namespace MeshLoader

#endif
