#include "font.hpp"
// #include "../render/texture.hpp"
#include "logging.hpp"
#include "util/files.hpp"
#include "util/png_image.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

// ^ what type of mad man write this?
//

namespace gui {

namespace render {

namespace structures {

// FontHandler::FontHandler () {
//     FT_Library ft;

//     if (FT_Init_FreeType(&ft)) {
//         LOG_WARNING(logging::main_logger, "Could not initiate FreeType.");
//         return;
//     }
// }

///

FontTexture::FontTexture(std::filesystem::path font_file) {
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        LOG_WARNING(logging::main_logger, "Could not initiate FreeType.");
        return;
    }

    FT_Face font_face;
    if (FT_New_Face(ft, font_file.c_str(), 0, &font_face)) {
        LOG_WARNING(logging::main_logger, "Could not load Pixelated Font");
        return;
    }

    FT_Set_Pixel_Sizes(font_face, 0, 20);

    if (FT_Load_Char(font_face, 'a', FT_LOAD_RENDER)) {
        LOG_WARNING(logging::main_logger, "Failed to load \"{}\" from font.", 'a');
        return;
    }

    auto settings = gpu_data::TextureSettings{
        .internal_format = gpu_data::GPUPixelStorageFormat::RED,
        .read_format = gpu_data::GPUPixelReadFormat::RED,
        .type = gpu_data::GPUPixelType::UNSIGNED_BYTE,
        .min_filter = GL_NEAREST,
        .mag_filter = GL_NEAREST};

    unsigned int max_height = 0;
    unsigned int total_width = 0;

    std::unordered_map<char, util::image::ByteMonochromeImage> images;

    descender_height_ = -font_face->descender;
    ascender_height_ = -font_face->ascender;
    text_height_ = font_face->ascender + descender_height_;

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(font_face, c, FT_LOAD_RENDER | FT_LOAD_MONOCHROME)) {
            LOG_WARNING(logging::main_logger, "Failed to load \"{}\" from font.", c);
            return;
        }

        auto char_size =
            glm::uvec2(font_face->glyph->bitmap.width, font_face->glyph->bitmap.rows);
        auto char_position =
            glm::uvec2(font_face->glyph->bitmap_left, font_face->glyph->bitmap_top);
        // auto char_texture = gpu_data::Texture2D(char_size.x, char_size.y, settings);

        std::vector<png_byte> data;
        data.resize(char_size.x * char_size.y);
        for (size_t i = 0; i < font_face->glyph->bitmap.width; i++) {
            for (size_t j = 0; j < font_face->glyph->bitmap.rows; j++) {
                png_byte font_bit =
                    font_face->glyph->bitmap
                        .buffer[j * font_face->glyph->bitmap.pitch + i / 8];
                uint8_t one = 1;
                uint8_t value = font_bit >> (7 - i % 8);
                data[i * font_face->glyph->bitmap.rows + j] = (value & one) * 255;
            }
        }
        //        if (c == 'a' || c == 'b') {
        //            LOG_DEBUG(logging::main_logger, "{}", data);
        //        }

        images.emplace(
            c, util::image::ByteMonochromeImage(
                   data.data(), char_size.x, char_size.y, sizeof(char)
               )
        );

        //        image::write_image(images.at(c), files::get_log_path() /
        //        ("font_as_image_" + std::string(1, c) + ".png"));

        font_textures_.emplace(
            c,
            Character{
                .size = char_size,
                .bearing = char_position,
                .advance = static_cast<unsigned int>(font_face->glyph->advance.x),
                .position_in_texture =
                    glm::ivec4(total_width, 0, total_width + char_size.x, char_size.y)}
        );

        total_width += char_size.x;
        if (char_size.y > max_height) {
            max_height = char_size.y;
        }
    }

    // std::shared_ptr<char> image(new char[max_height * total_width]);

    auto image = std::make_shared<util::image::ByteMonochromeImage>(
        total_width, max_height, sizeof(char)
    );

    for (unsigned char c = 0; c < 128; c++) {
        image->draw_at(
            images.at(c), font_textures_[c].position_in_texture.x,
            font_textures_[c].position_in_texture.y
        );
    }

    LOG_DEBUG(logging::main_logger, "saving fonts to file.");
/*    auto result_1 =
        image::write_image(*image, files::get_log_path() / "font_as_image.png");
    if (result_1 != image::write_result_t::WR_OK) {
        image::log_result(result_1, files::get_log_path() / "font_as_image.png");
    }*/

    image->transpose();
    texture_ = std::make_shared<gui::gpu_data::Texture2D>(image, settings);

    GlobalContext& global_context = GlobalContext::instance();
    global_context.run_opengl_queue();

    /*
    auto font_image_from_gpu = texture_->get_image();
    
    if (auto font_image_from_gpu_typed =
            std::dynamic_pointer_cast<util::image::ByteMonochromeImage>(
                font_image_from_gpu
            )) {
        font_image_from_gpu_typed->transpose();
        auto result_2 = image::write_image(
            *font_image_from_gpu_typed,
            files::get_log_path() / "font_as_image_from_texture.png"
        );
        if (result_2 != image::write_result_t::WR_OK) {
            image::log_result(
                result_2, files::get_log_path() / "font_as_image_from_texture.png"
            );
        }
    }
    */
}

} // namespace structures

} // namespace render

} // namespace gui
