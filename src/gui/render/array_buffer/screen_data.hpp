#pragma once

#include "array_buffer.hpp"
#include "gpu_data.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

namespace data_structures {

class ScreenData : public virtual GPUData {
 private:
    ArrayBuffer vertex_array_;
    //    ArrayBuffer element_array_;
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
     * @brief Get the vertex buffer id
     *
     * @return ArrayBuffer id of vertex buffer on gpu
     */
    inline const ArrayBuffer
    get_vertex_buffer() const {
        return vertex_array_;
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

    inline virtual void
    bind() const {
        vertex_array_.bind(0, 0);
        //        element_array_.bind(-1, -1);
    };

    inline virtual void
    release() const {
        glDisableVertexAttribArray(0);
    }

    inline virtual bool
    do_render() const {
        return true;
    };

};

} // namespace data_structures

} // namespace gui