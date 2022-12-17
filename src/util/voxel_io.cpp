#include "voxel_io.hpp"

#include "../exceptions.hpp"
#include "../logging.hpp"

#include <cinttypes>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static quill::Logger* logger = logging::get_logger("voxel_io");

namespace voxel_utility {

void
from_qb(
    const std::string path, std::vector<uint32_t>& data, std::vector<int>& center,
    std::vector<uint32_t>& size
) {
    LOG_INFO(logger, "Reading voxels from {}", path);

    // Read the tiles from the path specified, and save
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) {
        LOG_ERROR(logger, "Could not open {}. Are you in the write directory?", path);
        fclose(file);
        throw exc::file_not_found_error(path);
    }

    // int x, y, z; // position in terrain
    uint32_t void_,
        compression; // void int 32 used to read 32 bites without saving it.
    uint8_t v[4];

    LOG_TRACE_L1(logger, "Reading file header");
    READ<uint32_t>(void_, file);       // version
    READ<uint32_t>(void_, file);       // color format RGBA
    READ<uint32_t>(void_, file);       // orientation right handed // c
    READ<uint32_t>(compression, file); // no compression
    READ<uint32_t>(void_, file);       // vmask
    READ<uint32_t>(void_, file);
    //  none of these are used

    LOG_TRACE_L1(logger, "Reading file name");

    int8_t name_len;
    READ<int8_t>(name_len, file);

    LOG_DEBUG(logger, "File name length: {}", name_len);

    std::string name(name_len, '\0');
    fread(name.data(), sizeof(char), name_len, file);

    LOG_INFO(logger, "Filename: {}", name);

    // Get voxel grid size
    uint32_t X_max, Y_max, Z_max;

    READ<uint32_t>(X_max, file); // x
    READ<uint32_t>(Z_max, file); // z
    READ<uint32_t>(Y_max, file); // y

    size = {X_max, Y_max, Z_max};
    data.resize(X_max * Y_max * Z_max);

    LOG_DEBUG(logger, "Voxel grid size: {X} x {Y} x {Z}", X_max, Y_max, Z_max);

    // Get voxel grid center
    int32_t x_center, y_center, z_center;

    READ<int32_t>(x_center, file); // x
    READ<int32_t>(z_center, file); // z
    READ<int32_t>(y_center, file); // y

    center = {x_center, y_center, z_center};

    LOG_DEBUG(
        logger, "Voxel grid center: ({X}, {Y}, {Z})", x_center, y_center, z_center
    );

    size_t voxels_read = 0;
    if (compression) {
        LOG_ERROR(logger, "Cannot parse voxel files with compression");
        throw exc::not_implemented_error("Cannot parse compressed voxel files");
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

    LOG_INFO(logger, "Voxels read: {}", voxels_read);

    fclose(file);
}

VoxelObject::VoxelObject(const std::string path) {
    try {
        from_qb(path, data_, center_, size_);
        ok_ = true;
    } catch (const std::exception& e) {
        LOG_ERROR(logger, "Could not create VoxelObject: {}", e.what());
        ok_ = false;
    }
}

} // namespace voxel_utility
