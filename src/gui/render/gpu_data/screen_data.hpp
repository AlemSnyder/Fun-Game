#pragma once

#include "array_buffer.hpp"
#include "gpu_data.hpp"

#include <glm/glm.hpp>

namespace gui {

namespace gpu_data {

class ScreenData : public virtual GPUData {
 private:
    ArrayBuffer<glm::vec3> vertex_array_;
    unsigned int num_vertices_;

 public:
    /**
     * @brief Deleted copy constructor
     */
    ScreenData(const ScreenData& obj) = delete;

    /**
     * @brief Deleted copy operator
     */
    ScreenData& operator=(const ScreenData& obj) = delete;
    /**
     * @brief Construct a new Screen Data object, default constructor
     *
     */
    ScreenData();

    inline virtual ~ScreenData() {}

    /**
     * @brief Get the vertex buffer id
     *
     * @return ArrayBuffer id of vertex buffer on gpu
     */
    inline const ArrayBuffer<glm::vec3>&
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

} // namespace gpu_data

} // namespace gui
