#pragma once

#include "../types.hpp"
#include "../logging.hpp"
#include "../exceptions.hpp"
#include "bits.hpp"
#include "voxel.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace voxel_utility {
// TODO make this file 1k lines long

template <std::integral T>
inline void
read_int(std::ifstream& file, T& val) noexcept {
    file.read(reinterpret_cast<char*>(&val), sizeof(val));
}

template <std::integral T>
inline void
write_int(std::ofstream& file, T val) noexcept {
    file.write(reinterpret_cast<char*>(&val), sizeof(val));
}

inline ColorInt
parse_color(ColorInt color) {
    // Colors are saved in big endian format
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

inline ColorInt
export_color(ColorInt color) {
    // Colors are saved in big endian format
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

void from_qb(
    const std::filesystem::path path, std::vector<ColorInt>& data,
    glm::i32vec3& center, glm::u32vec3& size
);

template<VoxelLike T>
void
to_qb(const std::filesystem::path& path, const T& ter, bool compression = false) {
    LOG_INFO(logging::file_io_logger, "Saving voxels to {}.", path.string());

    if (compression) {
        LOG_ERROR(logging::file_io_logger, "Cannot write voxel files with compression");
        throw exc::not_implemented_error("Cannot write compressed voxel files");
    }

    // Saves the tiles in this to the path specified
    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file) {
        LOG_ERROR(
            logging::file_io_logger,
            "Could not open {}. Are you in the right directory?", path.string()
        );
        throw exc::file_not_found_error(path);
    }

    glm::u32vec3 size = ter.get_size();
    glm::i32vec3 offset = ter.get_offset();


    // Write header
    uint32_t count = 1; // the number of layers

    LOG_TRACE_L1(logging::file_io_logger, "Writing file header");

    write_int(file, 257U); // version
    write_int(file, 0U);   // color format RGBA
    write_int(file, 1U);   // orientation right handed // c
    write_int(file, 0U);   // no compression
    write_int(file, 0U);   // vmask
    write_int(file, count);

    // Write file name
    LOG_TRACE_L1(logging::file_io_logger, "Writing file name");

    const std::string name("Main World");
    write_int<int8_t>(file, name.length());
    file.write(name.c_str(), name.length());

    // Write voxel grid size
    LOG_DEBUG(
        logging::file_io_logger, "Voxel grid size: {X} x {Y} x {Z}",
        size[0], size[1], size[2]
    );

    write_int(file, size[0]); // x_max
    write_int(file, size[2]); // z_max
    write_int(file, size[1]); // y_max

    // Write voxel grid center
    LOG_DEBUG(
        logging::file_io_logger, "Voxel grid center: ({X}, {Y}, {Z})",
        offset[0], offset[1], offset[2]
    );

    write_int(file, offset[0]); // x_center
    write_int(file, offset[2]); // z_center
    write_int(file, offset[1]); // y_center

    // Write the voxels themselves
    LOG_DEBUG(logging::file_io_logger, "Writing voxels");

    size_t voxels_written = 0;
    for (size_t x = 0; x < size[0]; x++)
        for (size_t z = 0; z < size[2]; z++)
            for (size_t y = size[1] - 1; y < size[1]; y--) {
                ColorInt raw_color = export_color(ter.get_voxel(x, y, z));

                // Alpha is either 0 or 255
                if (raw_color & 0xff000000)
                    raw_color |= 0xff000000;

                write_int(file, raw_color);

                voxels_written++;
            }

    LOG_INFO(logging::file_io_logger, "Voxels written: {}", voxels_written);
}

inline qb_data from_qb(
    const std::filesystem::path& path
){
    std::vector<ColorInt> data;
    glm::i32vec3 center;
    glm::u32vec3 size;

    from_qb(path, data, center, size);

    return qb_data(data, center, size);

}

} // namespace voxel_utility
