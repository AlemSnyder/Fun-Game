#include "voxel_io.hpp"

#include "../exceptions.hpp"
#include "../logging.hpp"
#include "../types.hpp"
#include "bits.hpp"

#include <array>
#include <bit>
#include <cinttypes>
#include <concepts>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace voxel_utility {

void
from_qb(
    const std::filesystem::path path, std::vector<qb_layer_data_t> layer_data

    //, std::vector<ColorInt>& data, VoxelOffset& center,
    // VoxelSize& size
) {
    LOG_INFO(logging::file_io_logger, "Reading voxels from {}.", path.string());

    // Read the tiles from the path specified, and save
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        LOG_ERROR(
            logging::file_io_logger,
            "Could not open {}. Are you in the right directory?", path.string()
        );
        throw exc::file_not_found_error(path);
    }

    // Read file header
    uint32_t void_; // void int used to read 32 bites without saving it.
    uint32_t compression;
    uint32_t number_of_layers;

    LOG_TRACE_L1(logging::file_io_logger, "Reading file header");

    //  none of these are used
    read_int(file, void_);            // version
    read_int(file, void_);            // color format RGBA
    read_int(file, void_);            // orientation right handed // c
    read_int(file, compression);      // compression
    read_int(file, void_);            // vmask
    read_int(file, number_of_layers); // number of layers

    if (compression) {
        LOG_ERROR(logging::file_io_logger, "Cannot parse voxel files with compression");
        throw exc::not_implemented_error("Cannot parse compressed voxel files");
    }

    // everything after this line should be for each layer

    for (uint32_t l = 0; l < number_of_layers; l++) {

        std::vector<ColorInt> data;
        VoxelOffset center;
        VoxelSize size;

        // Read layer name
        LOG_TRACE_L1(logging::file_io_logger, "Reading voxel save name");

        int8_t name_len;
        read_int(file, name_len);

        LOG_TRACE_L1(logging::file_io_logger, "Voxel save name length: {}", name_len);

        std::string name(name_len, '\0');
        file.read(name.data(), name_len);

        LOG_TRACE_L1(logging::file_io_logger, "Voxel save name: {}", name);

        // Get voxel grid size
        uint32_t x_max, y_max, z_max;

        read_int(file, x_max); // x
        read_int(file, z_max); // z
        read_int(file, y_max); // y

        size = {x_max, y_max, z_max};
        data.resize(x_max * y_max * z_max);

        LOG_TRACE_L1(
            logging::file_io_logger, "Voxel grid size: {X} x {Y} x {Z}", x_max, y_max,
            z_max
        );

        // Get voxel grid center
        int32_t x_center, y_center, z_center;

        read_int(file, x_center); // x
        read_int(file, z_center); // z
        read_int(file, y_center); // y

        center = {x_center, y_center, z_center};

        LOG_DEBUG(
            logging::file_io_logger, "Voxel grid center: ({X}, {Y}, {Z})", x_center,
            y_center, z_center
        );

        // Read the voxels themselves
        LOG_DEBUG(logging::file_io_logger, "Reading voxels");

        size_t voxels_read = 0;
        for (size_t x = 0; x < size.x; x++)
            for (size_t z = 0; z < size.z; z++)
                for (size_t y = size.y - 1;; y--) {
                    uint32_t raw_color;

                    read_int(file, raw_color);
                    data[(x * y_max + y) * z_max + z] = parse_color(raw_color);

                    voxels_read++;
                    // so that nino doesn't kill me for using integer overflow.
                    if (y == 0) {
                        break;
                    }
                }

        LOG_INFO(logging::file_io_logger, "Voxels read: {}", voxels_read);

        layer_data.push_back(qb_layer_data_t(data, center, size, name));

    }
}

} // namespace voxel_utility
