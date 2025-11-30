#pragma once

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/texture.hpp"
#include "gui/render/gpu_data/vertex_array_object.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
#include "screen_data.hpp"

#include <filesystem>

namespace gui {
namespace render {

//    gpu_data::Texture2D texture;

//    std::array<uint8_t, 36> sub_texture_locations;

class WindowTexture : public virtual gpu_data::GPUData {
 private:
    // & to two things above
    gpu_data::VertexArrayObject vertex_array_object_;

    gpu_data::VertexBufferObject<glm::vec3> gl_positions_;
    gpu_data::VertexBufferObject<glm::vec2> screen_positions_;
    unsigned int num_vertices_;

 public:
    /**
     * @brief Deleted copy constructor
     */
    WindowTexture(const WindowTexture& obj) = delete;

    /**
     * @brief Deleted copy operator
     */
    WindowTexture& operator=(const WindowTexture& obj) = delete;

    /**
     * @brief Default move constructor
     */
    WindowTexture(WindowTexture&& obj) = default;

    /**
     * @brief Default move constructor
     */
    WindowTexture& operator=(WindowTexture&& obj) = default;

    /**
     * @brief Construct a new Screen Data object, default constructor
     *
     */
    WindowTexture();

    inline virtual ~WindowTexture() {}

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

    inline void
    update_position(std::array<screen_size_t, 4> positions) {
        screen_size_t x_diff = positions[2] - positions[0];
        screen_size_t y_diff = positions[3] - positions[1];

        std::vector<glm::vec2> vertex_data(
            {glm::vec2(x_diff, y_diff), glm::vec2(x_diff, 0), glm::vec2(0, y_diff),
             glm::vec2(0, 0)}
        );

        screen_positions_.update(vertex_data);
    }
};

} // namespace render

} // namespace gui
