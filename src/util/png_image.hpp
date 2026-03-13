// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file png_image.hpp
 *
 * @brief Defines image writer.
 *
 * @ingroup Util
 *
 */

#pragma once

#include "files.hpp"
#include "util/image.hpp"

#include <png.h>

#include <array>
#include <cstring>
#include <expected>
#include <filesystem>
#include <new>

namespace util {
namespace image {
class Image;
}
} // namespace util

namespace image {

enum write_result_t {
    WR_OK = 0,
    WR_FOPEN_FAILED,
    WR_CREATE_WRITE_STRUCT_FAILED,
    WR_CREATE_INFO_STRUCT_FAILED,
    WR_SETJMP_PNG_JMPBUF_FAILED,
    WR_ROW_MALLOC_FAILED,
};

template <class T>
concept ImageBW = requires(T const img, size_t i, size_t j) {
    { img.get_height() } -> std::convertible_to<size_t>;
    { img.get_width() } -> std::convertible_to<size_t>;
    { img.get_color(i, j) } -> std::same_as<png_byte>;
};

template <class T>
concept ImageColor = requires(T const img, size_t i, size_t j) {
    { img.get_height() } -> std::convertible_to<size_t>;
    { img.get_width() } -> std::convertible_to<size_t>;
    { img.get_color(i, j) } -> std::same_as<std::array<png_byte, 3>>;
};

template <class T>
concept ImageRGBA = requires(T const img, size_t i, size_t j) {
    { img.get_height() } -> std::convertible_to<size_t>;
    { img.get_width() } -> std::convertible_to<size_t>;
    { img.get_color(i, j) } -> std::same_as<std::array<png_byte, 4>>;
};

void log_result(write_result_t result, const std::filesystem::path& path);

// #if DEBUG()

class ImageTest {
 public:
    ImageTest() {};

    size_t
    get_height() const {
        return 1000;
    }

    size_t
    get_width() const {
        return 1000;
    }

    png_byte
    get_color(size_t i, size_t j) const {
        return static_cast<png_byte>(i * j);
    }
};

class PNG_write_info {
    public:
    png_structp png_ptr = nullptr;
    png_infop png_info = nullptr;

    PNG_write_info() {
        // Create our write struct
        // TODO these nullptr should be function pointers
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr) {return ;}
            // Create our info struct for this png image

        png_info = png_create_info_struct(png_ptr);
    }

    ~PNG_write_info() {
        auto temp_ptr_struct = (png_ptr) ? &png_ptr : nullptr;
        auto temp_png_info = (png_info) ? &png_info : nullptr;
        png_destroy_write_struct(temp_ptr_struct, temp_png_info);
    }
};

namespace {

template <class T, size_t n>
auto
to_array(std::array<T, n> array) {
    return array;
}

template <class T>
auto
to_array(T value) {
    return std::array<T, 1>({value});
}

template <class T, unsigned int n>
[[nodiscard]] write_result_t
write_image_base(T image, const std::filesystem::path& path /*other settings*/) {
    // Keep track of if we succeeded or not
    write_result_t status = WR_OK;

    // Get image information
    size_t WIDTH = image.get_width();
    size_t HEIGHT = image.get_height();

    char meta_lang[] = "en";
    char meta_key[] = "An Image";
    char meta_text[] = "Some text";


    // Open the file for writing
//    auto path_str = path.string(); // need to keep this from being free'd
    std::unique_ptr<std::FILE, void(*)(std::FILE*)> file (
        fopen(path.c_str(), "wb"), [](std::FILE* file){
            fclose(file);
        }
    );

    if (!file) {
        status = WR_FOPEN_FAILED;
        return status;
    }

    // Create write struct and info struct
    PNG_write_info info;

    if (!info.png_ptr) {
        status = WR_CREATE_WRITE_STRUCT_FAILED;
        return status;
    }

    if (!info.png_info) {
        status = WR_CREATE_INFO_STRUCT_FAILED;
        return status;
    }

    // Set up IO for our file
    png_init_io(info.png_ptr, file.get());

    int color_type;

    if constexpr (n == 1) {
        color_type = PNG_COLOR_TYPE_GRAY;
    } else if constexpr (n == 3) {
        color_type = PNG_COLOR_TYPE_RGB;
    } else if constexpr (n == 4) {
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    } else {
        static_assert(n == 1 || n == 3 || n == 4, "Invalid number of color channels");
    }

    // set information about our image
    png_set_IHDR(
        info.png_ptr, info.png_info, WIDTH, HEIGHT, 8, color_type, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    // Set metadata about the PNG file
    png_text meta_data;
    memset(&meta_data, 0, sizeof(meta_data)); // clear struct

    meta_data.compression = PNG_TEXT_COMPRESSION_NONE; // no compression
    meta_data.lang_key = meta_lang;
    meta_data.key = meta_key;
    meta_data.text = meta_text;

    png_set_text(info.png_ptr, info.png_info, &meta_data, 1);
    png_write_info(info.png_ptr, info.png_info);

    // multiple colors are written in the same row
    /*
     * write rows of image
     */
    size_t i, j;
    std::vector<png_byte> row(n * WIDTH);

    // write row data
    for (i = 0; i < HEIGHT; i++) {
        // set row data
        for (j = 0; j < WIDTH; j++) {
            const std::array<png_byte, n> pixel_color = to_array(image.get_color(j, i));
            for (unsigned int channel = 0; channel < n; channel++) {
                row[n * j + channel] = pixel_color[channel];
            }
        }

        // write the row
        png_write_row(info.png_ptr, row.data());
    }

    /*
     * Cleanups
     */
    // Finish our write
    png_write_end(info.png_ptr, info.png_info);

    return status;
}

} // namespace

template <ImageBW T>
[[nodiscard]] write_result_t
write_image(T image, const std::filesystem::path& path) {
    return write_image_base<T, 1>(image, path);
}

template <ImageColor T>
[[nodiscard]] write_result_t
write_image(T image, const std::filesystem::path& path) {
    return write_image_base<T, 3>(image, path);
}

template <ImageRGBA T>
[[nodiscard]] write_result_t
write_image(T image, const std::filesystem::path& path) {
    return write_image_base<T, 4>(image, path);
}

class ColorImageTest {
 public:
    ColorImageTest() {};

    size_t
    get_height() const {
        return 1000;
    }

    size_t
    get_width() const {
        return 1000;
    }

    std::array<png_byte, 3>
    get_color(size_t i, size_t j) const {
        return std::array<png_byte, 3>(
            {static_cast<png_byte>(i), static_cast<png_byte>(j), 10}
        );
    }
};

// #endif

[[nodiscard]] std::expected<util::image::ImageVariant, int>
read_image(std::filesystem::path path);

} // namespace image
