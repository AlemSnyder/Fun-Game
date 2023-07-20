#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace data_structures {

class ScreenData {
 private:
    GLuint vertex_buffer_;
    GLuint element_buffer_;
    unsigned int num_vertices_;

 public:
    /**
     * @brief Construct a new Screen Data object
     *
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     *
     * @param obj
     */
    ScreenData(const ScreenData& obj) = delete;

    /**
     * @brief The copy operator
     *
     * @warning You shouldn't do this. This will delete the buffer from the gpu
     *
     * @param obj
     * @return ScreenData&
     */
    ScreenData& operator=(const ScreenData& obj) = delete;
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
    inline GLuint
    get_vertex_buffer() const {
        return vertex_buffer_;
    }

    /**
     * @brief Get the element buffer id
     *
     * @deprecated this is 0,1,2,3 should probably be removed
     *
     * @return GLuint id of element buffer on gpu
     */
    inline GLuint
    get_element_buffer() const {
        return element_buffer_;
    }

    /**
     * @brief Get the number of vertices
     *
     * @return unsigned int 4 (it is a rectangle)
     */
    inline unsigned int
    get_num_vertices() const {
        return num_vertices_;
    }
};

} // namespace data_structures

} // namespace gui
