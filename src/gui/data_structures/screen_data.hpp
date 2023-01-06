#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

class ScreenData {
 private:
    GLuint vertex_buffer_;
    GLuint element_buffer_;
    unsigned int num_vertices_;

 public:
    /**
     * @brief Construct a new Screen Data object
     * 
     * @param obj 
     */
    inline ScreenData(const ScreenData& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
    };

    /**
     * @brief The copy operator
     * 
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     * 
     * @param obj 
     * @return ScreenData&
     */
    inline ScreenData& operator=(const ScreenData& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
        return *this;
    }
    /**
     * @brief Construct a new Screen Data object, default constructor
     * 
     */
    ScreenData();
    /**
     * @brief Destroy the Screen Data object
     * 
     */
    inline ~ScreenData() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &element_buffer_);
    }
    /**
     * @brief Get the vertex buffer id
     * 
     * @return GLuint id of vertex buffer on gpu
     */
    inline GLuint get_vertex_buffer() const { return vertex_buffer_; }
    /**
     * @brief Get the element buffer id
     * 
     * @deprecated this is 0,1,2,3 should probably be removed
     * 
     * @return GLuint id of element buffer on gpu
     */
    inline GLuint get_element_buffer() const { return element_buffer_; }
    /**
     * @brief Get the number of vertices
     * 
     * @return unsigned int 4 (it is a rectangle)
     */
    inline unsigned int get_num_vertices() const { return num_vertices_; }
};

} // namespace terrain
