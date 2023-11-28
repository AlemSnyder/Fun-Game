#pragma once

#include "files.hpp"

#include <png.h>

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

class image_bw {
 public:
    virtual size_t get_height() const = 0;
    virtual size_t get_width() const = 0;
    virtual png_byte get_color(size_t i, size_t j) const = 0;
};

template <class T>
concept image_bw_c = std::is_base_of<image_bw, T>::value;

template <image_bw_c T>
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
    // @egelja what is this?
    // I removed it and things still work. Adding it breaks things.
    //memset(&meta_data, 0, sizeof(meta_data)); // clear struct

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
            row[j] = image.get_color(i, j);

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

int
image_result_logger(write_result_t result, const std::filesystem::path& path);

// #if DEBUG()

class ImageTest : public image_bw{
 public:
    ImageTest(){};

    size_t
    get_height() const override {
        return 1000;
    }

    size_t
    get_width() const override {
        return 1000;
    }

    png_byte
    get_color(size_t i, size_t j) const override {
        return static_cast<png_byte>(i * j);
    }
};

// #endif

} // namespace image
