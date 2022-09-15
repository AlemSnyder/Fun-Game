#include "voxel_io.hpp"

#include <cinttypes>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace voxel_utility {

int
from_qb(
    const std::string path, std::vector<uint32_t>& data, std::vector<int>& center,
    std::vector<uint32_t>& size
) {
    // Read the tiles from the path specified, and save
    FILE* file;
    file = fopen(path.c_str(), "rb");
    if (!file) {
        std::cerr << "Impossible to open " << path
                  << ". Are you in the right directory?" << std::endl;
        getchar();
        fclose(file);
        return 1;
    }

    // This is partially from goxel with GPL license
    std::cout << "Reading form " << path << "\n";
    // int x, y, z; // position in terrain
    uint32_t void_, compression; // void int 32 used to read 32 bites without saving it.
    int32_t x_center, y_center, z_center;
    uint8_t v[4];

    READ<uint32_t>(void_, file); // version
    // std::cout << void_ << std::endl;
    READ<uint32_t>(void_, file); // color format RGBA
    // std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file); // orientation right handed // c
    // std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(compression, file); // no compression
    // std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file); // vmask
    // std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);
    // std::cout << void_ << std::endl;(void) void_;
    //  none of these are used

    int8_t name_len;
    READ<int8_t>(name_len, file);
    std::cout << "name length: " << static_cast<int>(name_len) << std::endl;
    char* name = new char[name_len];
    fread(name, sizeof(char), name_len, file);
    std::string string_name(name);
    std::cout << "Name: " << string_name << std::endl;
    uint32_t X_max, Y_max, Z_max;
    READ<uint32_t>(X_max, file); // x
    READ<uint32_t>(Z_max, file); // z
    READ<uint32_t>(Y_max, file); // y
    size = {X_max, Y_max, Z_max};
    READ<int32_t>(x_center, file); // x
    READ<int32_t>(z_center, file); // z
    READ<int32_t>(y_center, file); // y
    center = {x_center, y_center, z_center};
    std::cout << "    max X: " << X_max << std::endl;
    std::cout << "    max Y: " << Y_max << std::endl;
    std::cout << "    max Z: " << Z_max << std::endl;
    data.resize(X_max * Y_max * Z_max);
    int tiles_read = 0;
    if (static_cast<bool>(compression)) {
        std::cout << "There is compression!" << std::endl;
    } else {
        data.resize(size[0] * size[1] * size[2]);
        for (size_t x = 0; x < size[0]; x++)
            for (size_t z = 0; z < size[2]; z++)
                for (size_t y = size[1] - 1; y < size[1]; y--) {
                    // int32_t* color = (int32_t*) malloc(32);
                    fread(v, sizeof(uint8_t), 4, file);
                    // fread(&color, 32, 1, file); // read the color
                    data[(x * static_cast<int>(size[1]) + y) * size[2] + z] =
                        compress_color(v);
                    tiles_read++;
                }
    }
    std::cout << "Voxels read: " << tiles_read << std::endl;
    // free(void_);
    // free(compression);
    // free(name_len);
    // free(name);
    delete[] name;

    fclose(file);
    return 0;
}

VoxelObject::VoxelObject(const std::string path) {
    data_ = std::vector<uint32_t>(0);
    center_ = std::vector<int>(0);
    size_ = std::vector<uint32_t>(0);

    int test = from_qb(path, data_, center_, size_);
    ok_ = (test == 0);
}

VoxelObject::VoxelObject(const std::filesystem::path fs_path) {
    std::string path = fs_path.string();
    data_ = std::vector<uint32_t>(0);
    center_ = std::vector<int>(0);
    size_ = std::vector<uint32_t>(0);

    int test = from_qb(path, data_, center_, size_);
    ok_ = (test == 0);
}

} // namespace voxel_utility
