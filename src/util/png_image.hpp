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

#include <png.h>

#include <array>
#include <cstring>
#include <filesystem>
#include <new>

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

template <ImageBW T>
[[nodiscard]] write_result_t
write_image(T image, const std::filesystem::path& path) {
    // Keep track of if we succeeded or not
    write_result_t status = WR_OK;

    // Get image information
    size_t WIDTH = image.get_width();
    size_t HEIGHT = image.get_height();

    char meta_lang[] = "en";
    char meta_key[] = "An Image";
    char meta_text[] = "Some text";

    // Create png variables
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;

    // Open the file for writing
    auto path_str = path.string(); // need to keep this from being free'd
    std::FILE* file = fopen(path_str.c_str(), "wb");

    if (!file) {
        status = WR_FOPEN_FAILED;
        goto fopen_failed;
    }

    // Create our write struct
    // TODO these nullptr should be function pointers
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        status = WR_CREATE_WRITE_STRUCT_FAILED;
        goto png_create_write_struct_failed;
    }

    // Create our info struct for this png image
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        status = WR_CREATE_INFO_STRUCT_FAILED;
        goto png_create_info_struct_failed;
    }

    // Set jump buffer for callbacks
    if (setjmp(png_jmpbuf(png_ptr))) {
        status = WR_SETJMP_PNG_JMPBUF_FAILED;
        goto setjmp_png_jmpbuf_failed;
    }

    // Set up IO for our file
    png_init_io(png_ptr, file);

    // set information about our image
    png_set_IHDR(
        png_ptr, info_ptr, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    // Set metadata about the PNG file
    png_text meta_data;
    memset(&meta_data, 0, sizeof(meta_data)); // clear struct

    meta_data.compression = PNG_TEXT_COMPRESSION_NONE; // no compression
    meta_data.lang_key = meta_lang;
    meta_data.key = meta_key;
    meta_data.text = meta_text;

    png_set_text(png_ptr, info_ptr, &meta_data, 1);
    png_write_info(png_ptr, info_ptr);

    // multiple colors are written in the same row
    // not sure how I want to implement this in a template safe way
    // https://stackoverflow.com/questions/48757099/write-an-image-row-by-row-with-libpng-using-c

    /*
     * write rows of image
     */
    size_t i, j;
    png_bytep row;

    // allocate data for row
    row = new (std::nothrow) png_byte[WIDTH];
    if (!row) {
        status = WR_ROW_MALLOC_FAILED;
        goto row_malloc_failed;
    }

    // write row data
    for (i = 0; i < HEIGHT; i++) {
        // set row data
        for (j = 0; j < WIDTH; j++)
            row[j] = static_cast<png_byte>(image.get_color(i, j));

        // write the row
        png_write_row(png_ptr, row);
    }

    /*
     * Cleanups
     */
    // Free our row data
    delete[] row;

row_malloc_failed:
    // Finish our write
    png_write_end(png_ptr, info_ptr);

setjmp_png_jmpbuf_failed:
png_create_info_struct_failed:
    // Free our write struct
    png_destroy_write_struct(&png_ptr, &info_ptr);

png_create_write_struct_failed:
    // Close our file
    fclose(file);

fopen_failed:
    return status;
}

void log_result(write_result_t result, const std::filesystem::path& path);

// #if DEBUG()

class ImageTest {
 public:
    ImageTest(){};

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

template <ImageColor T>
[[nodiscard]] write_result_t
write_image(T image, const std::filesystem::path& path) {
    // Keep track of if we succeeded or not
    write_result_t status = WR_OK;

    // Get image information
    size_t WIDTH = image.get_width();
    size_t HEIGHT = image.get_height();

    char meta_lang[] = "en";
    char meta_key[] = "An Image";
    char meta_text[] = "Some text";

    // Create png variables
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;

    // Open the file for writing
    auto path_str = path.string(); // need to keep this from being free'd
    std::FILE* file = fopen(path_str.c_str(), "wb");

    if (!file) {
        status = WR_FOPEN_FAILED;
        goto fopen_failed;
    }

    // Create our write struct
    // TODO these nullptr should be function pointers
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        status = WR_CREATE_WRITE_STRUCT_FAILED;
        goto png_create_write_struct_failed;
    }

    // Create our info struct for this png image
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        status = WR_CREATE_INFO_STRUCT_FAILED;
        goto png_create_info_struct_failed;
    }

    // Set jump buffer for callbacks
    if (setjmp(png_jmpbuf(png_ptr))) {
        status = WR_SETJMP_PNG_JMPBUF_FAILED;
        goto setjmp_png_jmpbuf_failed;
    }

    // Set up IO for our file
    png_init_io(png_ptr, file);

    // set information about our image
    png_set_IHDR(
        png_ptr, info_ptr, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    // Set metadata about the PNG file
    png_text meta_data;
    memset(&meta_data, 0, sizeof(meta_data)); // clear struct

    meta_data.compression = PNG_TEXT_COMPRESSION_NONE; // no compression
    meta_data.lang_key = meta_lang;
    meta_data.key = meta_key;
    meta_data.text = meta_text;

    png_set_text(png_ptr, info_ptr, &meta_data, 1);
    png_write_info(png_ptr, info_ptr);

    // multiple colors are written in the same row
    // not sure how I want to implement this in a template safe way
    // https://stackoverflow.com/questions/48757099/write-an-image-row-by-row-with-libpng-using-c

    /*
     * write rows of image
     */
    size_t i, j;
    png_bytep row;

    // allocate data for row
    row = new (std::nothrow) png_byte[3 * WIDTH];
    if (!row) {
        status = WR_ROW_MALLOC_FAILED;
        goto row_malloc_failed;
    }

    // write row data
    for (i = 0; i < HEIGHT; i++) {
        // set row data
        for (j = 0; j < WIDTH; j++) {
            const std::array<png_byte, 3> pixel_color = image.get_color(i, j);
            row[3 * j] = pixel_color[0];
            row[3 * j + 1] = pixel_color[1];
            row[3 * j + 2] = pixel_color[2];
        }

        // write the row
        png_write_row(png_ptr, row);
    }

    /*
     * Cleanups
     */
    // Free our row data
    delete[] row;

row_malloc_failed:
    // Finish our write
    png_write_end(png_ptr, info_ptr);

setjmp_png_jmpbuf_failed:
png_create_info_struct_failed:
    // Free our write struct
    png_destroy_write_struct(&png_ptr, &info_ptr);

png_create_write_struct_failed:
    // Close our file
    fclose(file);

fopen_failed:
    return status;
}

class ColorImageTest {
 public:
    ColorImageTest(){};

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

} // namespace image
