#pragma once

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/vertex_array_object.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"

#include <glm/glm.hpp>

namespace gui {

namespace gpu_data {

class ScreenData : public virtual GPUData {
 private:
    VertexArrayObject vertex_array_object_;

    VertexBufferObject<glm::vec3> vertex_array_;
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

    /**
     * @brief Get the vertex buffer id
     *
     * @return VertexBufferObject id of vertex buffer on gpu
     */
    inline const VertexBufferObject<glm::vec3>&
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
        vertex_array_object_.bind();
    };

    inline virtual void
    release() const {
        vertex_array_object_.release();
    }

    inline virtual bool
    do_render() const {
        return true;
    };
};

} // namespace gpu_data

} // namespace gui
