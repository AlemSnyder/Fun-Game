// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file font.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines FontTexture Class
 *
 * @ingroup GUI  RENDER  STRUCTURES
 *
 */

#pragma once

#include "../gpu_data/texture.hpp"

#include <memory>
#include <vector>

namespace gui {

namespace render {

namespace structures {

/**
 * @brief Data that defines size and location in texture of a character.
 */
struct Character {
    glm::uvec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
    glm::ivec4 position_in_texture;
};

/**
 * @brief Font data saved as an OpenGL texture.
 */
class FontTexture {
 private:
    std::shared_ptr<gpu_data::Texture2D> texture_;

    // map of char to location (represented as a Character) in texture of the rasterred
    // font
    std::unordered_map<char, Character> font_textures_;

    // Maximum height of any character in pixels.
    uint16_t text_height_;
    // the distance from the baseline to the top of the character
    uint16_t ascender_height_;
    // Distance from baseline to bottom or character
    uint16_t descender_height_;

 public:
    /**
     * @brief Construct a new FontTexture Object from file.
     *
     * @param std::filesystem::path font_file file to read font from.
     */
    FontTexture(std::filesystem::path font_file);

    /**
     * @brief Bind to texture index
     *
     * @details texture_index is the input in a shader program ie. GL_TEXTURE#.
     *
     * @param GLuint texture_index index of texture
     */
    inline virtual void
    bind(GLuint texture_index) const {
        texture_->bind(texture_index);
    }

    /**
     * @brief Get pointer to Character for given char
     *
     * @param char character Character to quarry texture location of.
     *
     * @return const Character* pointer to location in texture of rasterred character or
     * nullptr if not found.
     */
    [[nodiscard]] inline const Character*
    get_character(char character) const {
        const auto data = font_textures_.find(character);
        if (data != font_textures_.end()) {
            return &data->second;
        } else {
            LOG_WARNING(
                logging::main_logger,
                "Could not find character \"{}\" with value \"{}\" "
                "in font.",
                std::string(1, character), static_cast<int>(character)
            );
            return nullptr;
        }
    }

    /**
     * @brief Get the height of the font
     *
     * @return uint16_t height of text
     */
    [[nodiscard]] inline auto
    get_text_height() const {
        return text_height_;
    }

    /**
     * @brief Get the ascender height
     *
     * @return uint16_t ascender height
     */
    [[nodiscard]] inline auto
    get_ascender_height() const {
        return ascender_height_;
    }

    /**
     * @brief Get the descender height
     *
     * @return uint16_t descender height
     */
    [[nodiscard]] inline auto
    get_descender_height() const {
        return descender_height_;
    }
};

} // namespace structures

} // namespace render

} // namespace gui
