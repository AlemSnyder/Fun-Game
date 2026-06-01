#include "window_texture.hpp"

#include "logging.hpp"
#include "util/files.hpp"
#include "util/png_image.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace render {

WindowTexture::WindowTexture(
    util::image::ImageVariant image, glm::ivec4 border_size,
    glm::ivec4 side_lengths, glm::ivec2 inner_pattern_size,
    std::array<glm::ivec2, 9> texture_regions
) :
    // clang-format off
    gl_positions_{
        glm::vec3(-1, -1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, -1, 0),
        glm::vec3(1, 1, 0)
    },
    screen_positions_{
        glm::vec2(-1, -1),
        glm::vec2(-1, 1),
        glm::vec2(1, -1),
        glm::vec2(0, 0)},
    // clang-format on
    num_vertices_(4),
    border_texture_(
        image,
        gui::gpu_data::TextureSettings{
            .internal_format = gui::gpu_data::GPUPixelStorageFormat::RGBA8UI,
            .read_format = gui::gpu_data::GPUPixelReadFormat::RGBA_INTEGER,
            .type = gui::gpu_data::GPUPixelType::UNSIGNED_BYTE,
            .min_filter = GL_NEAREST,
            .mag_filter = GL_NEAREST
        }
    ),
    border_size_(border_size), side_lengths_(side_lengths),
    inner_pattern_size_(inner_pattern_size), texture_regions_(texture_regions) {
    LOG_DEBUG(logging::main_logger, "Initializing a WindowTexture");
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this]() {
        vertex_array_object_.bind();
        gl_positions_.attach_to_vertex_attribute(0);
        screen_positions_.attach_to_vertex_attribute(1);
        vertex_array_object_.release();
    });
}

WindowTexture::WindowTexture(
    util::image::ImageVariant image, const window_texture_data_t& texture_data
) :
    WindowTexture(
        image, texture_data.border_size, texture_data.side_lengths,
        texture_data.inner_pattern_size, texture_data.texture_regions
    ) {}

} // namespace render

} // namespace gui
