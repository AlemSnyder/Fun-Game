#pragma once

#include "files.hpp"

#include <png.h>

#include <filesystem>

namespace image {

// T should be int8_t for grayscale, or other for other types
template <class T>
class ImageBase {
 public:
    virtual T get(size_t i, size_t j) = 0;
    virtual size_t get_height() = 0;
    virtual size_t get_width() = 0;
};

template <class T>
void
write_image(T image, std::filesystem::path path) {
    //if (!std::filesystem::exists(path)) [[unlikely]] {
    //    return;
    //}

    std::FILE* file = fopen(path.string().data(), "wb");

    // god a hate c. How is one supposed to know this is a pointer?
    // TODO these nullptr should be function pointers
    png_structp png_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (png_ptr == nullptr) {
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(file);
        return;
    }

    png_init_io(png_ptr, file);

    size_t WIDTH = image.get_width();
    size_t HEIGHT = image.get_height();

    png_set_IHDR(
        png_ptr, info_ptr, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    png_text meta_data;
    meta_data.compression = PNG_TEXT_COMPRESSION_NONE;
    meta_data.lang_key = const_cast<char *>("en");
    meta_data.key = const_cast<char *>("An Image");
    meta_data.text = const_cast<char *>("Some text");
    png_set_text(png_ptr, info_ptr, &meta_data, 1);
    png_write_info(png_ptr, info_ptr);

    // multiple colors are written in the same row
    // not sure how I want to implement this
    // https://stackoverflow.com/questions/48757099/write-an-image-row-by-row-with-libpng-using-c

    png_bytep row = (png_bytep)malloc(WIDTH * sizeof(png_byte));
    for (size_t i = 0; i < HEIGHT; i++) {
        // TODO free this memory
        for (size_t j = 0; j < WIDTH; j++) {
            row[j] = image.get(i, j);
        }
        // row_pointers[i] = row;
        png_write_row(png_ptr, row);
    }
    free(row);

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

// #if DEBUG()

class ImageTest {
 public:
    ImageTest(){};

    size_t
    get_height() {
        return 1000;
    }

    size_t
    get_width() {
        return 1000;
    }

    png_byte
    get(size_t i, size_t j) {
        return static_cast<png_byte>(i * j);
    }
};

// #endif

} // namespace image
