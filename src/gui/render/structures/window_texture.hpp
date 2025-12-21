#pragma once

#include "gui/render/gpu_data/data_types.hpp"
#include "gui/render/gpu_data/texture.hpp"
#include "gui/render/gpu_data/vertex_array_object.hpp"
#include "gui/render/gpu_data/vertex_buffer_object.hpp"
#include "screen_data.hpp"
#include "util/image.hpp"

#include <glaze/glaze.hpp>

#include <array>
#include <filesystem>

namespace gui {

namespace render {

struct window_texture_data_t {
    std::filesystem::path texture_file;
    glm::ivec4 border_size;
    glm::ivec4 side_lengths;
    glm::ivec2 inner_pattern_size;
    std::array<glm::ivec2, 9> texture_regions;

    inline std::array<int, 4> border_size_write() {
        return {border_size[0], border_size[1],border_size[2],border_size[3]};
    }

        inline void border_size_read(std::array<int, 4> border_size_in) {
            border_size = glm::ivec4(border_size_in[0], border_size_in[1],border_size_in[2],border_size_in[3]);
    }

        inline std::array<int, 4> side_lengths_write() {
        return {side_lengths[0], side_lengths[1],side_lengths[2],side_lengths[3]};
    }

        inline void side_lengths_read(std::array<int, 4> side_lengths_in) {
            side_lengths = glm::ivec4(side_lengths_in[0], side_lengths_in[1],side_lengths_in[2],side_lengths_in[3]);
    }

    inline std::array<int, 2> inner_pattern_size_write() {
        return {inner_pattern_size[0], inner_pattern_size[1]};
    }

        inline void inner_pattern_size_read(std::array<int, 2> inner_pattern_size_in) {
            inner_pattern_size = glm::ivec2(inner_pattern_size_in[0], inner_pattern_size_in[1]);
    }

        inline std::array<std::array<int, 2>, 9> texture_regions_write() {
        return {std::array<int, 2>({texture_regions[0].x, texture_regions[0].y}),
                std::array<int, 2>({texture_regions[1].x, texture_regions[1].y}),
                std::array<int, 2>({texture_regions[2].x, texture_regions[2].y}),
                std::array<int, 2>({texture_regions[3].x, texture_regions[3].y}),
                std::array<int, 2>({texture_regions[4].x, texture_regions[4].y}),
                std::array<int, 2>({texture_regions[5].x, texture_regions[5].y}),
                std::array<int, 2>({texture_regions[6].x, texture_regions[6].y}),
                std::array<int, 2>({texture_regions[7].x, texture_regions[7].y}),
                std::array<int, 2>({texture_regions[8].x, texture_regions[8].y})};
    }

        inline void texture_regions_read(std::array<std::array<int, 2>, 9> texture_regions_in) {
            texture_regions = std::array<glm::ivec2, 9>({glm::ivec2(texture_regions_in[0][0], texture_regions_in[0][1]),
         glm::ivec2(texture_regions_in[1][0], texture_regions_in[1][1]),
         glm::ivec2(texture_regions_in[2][0], texture_regions_in[2][1]),
         glm::ivec2(texture_regions_in[3][0], texture_regions_in[3][1]),
         glm::ivec2(texture_regions_in[4][0], texture_regions_in[4][1]),
         glm::ivec2(texture_regions_in[5][0], texture_regions_in[5][1]),
         glm::ivec2(texture_regions_in[6][0], texture_regions_in[6][1]),
         glm::ivec2(texture_regions_in[7][0], texture_regions_in[7][1]),
         glm::ivec2(texture_regions_in[8][0], texture_regions_in[8][1])});
    }
};

class WindowTexture : public virtual gpu_data::GPUData {
 private:
    // & to two things above
    gpu_data::VertexArrayObject vertex_array_object_;

    gpu_data::VertexBufferObject<glm::vec3> gl_positions_;
    gpu_data::VertexBufferObject<glm::vec2> screen_positions_;
    unsigned int num_vertices_;

    gpu_data::Texture2D border_texture_;

    glm::ivec4 border_size_;
    glm::ivec4 side_lengths_;
    glm::ivec2 inner_pattern_size_;
    std::array<glm::ivec2, 9> texture_regions_;

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
    WindowTexture(
        std::shared_ptr<util::image::Image> image, glm::ivec4 border_size,
        glm::ivec4 side_lengths, glm::ivec2 inner_pattern_size,
        std::array<glm::ivec2, 9> texture_regions
    );

    WindowTexture(std::shared_ptr<util::image::Image> image, const window_texture_data_t& texture_data);

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

    [[nodiscard]] inline auto get_border_size() const {
        return border_size_;
    }
    [[nodiscard]] inline auto get_side_lengths() const {
        return side_lengths_;
    }
    [[nodiscard]] inline auto get_inner_pattern_size() const {
        return inner_pattern_size_;
    }
    [[nodiscard]] inline auto get_texture_regions() const {
        return texture_regions_;
    }

    inline virtual void
    bind() const {
        vertex_array_object_.bind();
        border_texture_.bind(0);
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

template <>
struct glz::meta<gui::render::window_texture_data_t> {
    using T = gui::render::window_texture_data_t;

    static constexpr auto value = object("texture_file", &T::texture_file,
                                         "border_size", custom<&T::border_size_read, &T::border_size_write>,
                                         "side_lengths", custom<&T::side_lengths_read, &T::side_lengths_write>,
                                         "inner_pattern_size", custom<&T::inner_pattern_size_read, &T::inner_pattern_size_write>,
                                         "texture_regions", custom<&T::texture_regions_read, &T::texture_regions_write>);
};

