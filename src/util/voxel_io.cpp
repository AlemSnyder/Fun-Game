#include "voxel_io.hpp"
#include "../logging.hpp"

#include <cinttypes>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//static quill::Logger* logger = logging::get_logger("util.voxel_io");

namespace voxel_utility {

//TODO path should be a file system path not string
int
from_qb(
    const std::string path, std::vector<uint32_t>& data, std::vector<int>& center,
    std::vector<uint32_t>& size
) {
    // Read the tiles from the path specified, and save
    FILE* file;
    file = fopen(path.c_str(), "rb");
    if (!file) {
        LOG_ERROR(logger, "Impossible to open {}.", path);
        getchar();
        fclose(file);
        return 1;
    }

    // This is partially from goxel with GPL license
    LOG_INFO(logger, "Reading form {}", path);
    // int x, y, z; // position in terrain
    uint32_t void_, compression; // void int 32 used to read 32 bites without saving it.
    int32_t x_center, y_center, z_center;
    uint8_t v[4];

    READ<uint32_t>(void_, file); // version
    READ<uint32_t>(void_, file); // color format RGBA
    READ<uint32_t>(void_, file); // orientation right handed // c
    READ<uint32_t>(compression, file); // no compression
    READ<uint32_t>(void_, file); // vmask
    READ<uint32_t>(void_, file);
    //  none of these are used

    int8_t name_len;
    READ<int8_t>(name_len, file);
    LOG_DEBUG(logger, "Name Length: {}", static_cast<int>(name_len));
    char* name = new char[name_len];
    fread(name, sizeof(char), name_len, file);
    std::string string_name(name);
    LOG_DEBUG(logger, "Name: {}", string_name);
    uint32_t X_max, Y_max, Z_max;
    READ<uint32_t>(X_max, file); // x
    READ<uint32_t>(Z_max, file); // z
    READ<uint32_t>(Y_max, file); // y
    size = {X_max, Y_max, Z_max};
    READ<int32_t>(x_center, file); // x
    READ<int32_t>(z_center, file); // z
    READ<int32_t>(y_center, file); // y
    center = {x_center, y_center, z_center};
    LOG_DEBUG(logger, "Max X: {}", X_max);
    LOG_DEBUG(logger, "Max Y: {}", Y_max);
    LOG_DEBUG(logger, "Max Z: {}", Z_max);
    data.resize(X_max * Y_max * Z_max);
    int voxels_read = 0;
    if (static_cast<bool>(compression)) {
        LOG_DEBUG(logger, "Using compression.");
        LOG_ERROR(logger, "This is not implemented.");
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
                    voxels_read++;
                }
    }
    LOG_DEBUG(logger, "Voxels read: {}", voxels_read);

    // free(void_);
    // free(compression);
    // free(name_len);
    // free(name);
    delete[] name;

    fclose(file);
    return 0;
}

VoxelObject::VoxelObject(const std::string path) {
    int test = from_qb(path, data_, center_, size_);
    ok_ = (test == 0);
}

VoxelObject::VoxelObject(const std::filesystem::path fs_path) {
    int test = from_qb(fs_path.string(), data_, center_, size_);
    ok_ = (test == 0);
}

} // namespace voxel_utility
