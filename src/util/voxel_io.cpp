#include "voxel_io.hpp"

#include "../exceptions.hpp"
#include "../logging.hpp"
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

template <std::integral T>
static inline void
read_int(std::ifstream& file, T& val) noexcept
{
    file.read(reinterpret_cast<char*>(&val), sizeof(val));
}

template <std::integral T>
static inline void
write_int(std::ofstream& file, T val) noexcept
{
    file.write(reinterpret_cast<char*>(&val), sizeof(val));
}

static inline uint32_t
parse_color(uint32_t color)
{
    // Colors are saved in big endian format
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

static inline uint32_t
export_color(uint32_t color)
{
    // Colors are saved in big endian format
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

namespace voxel_utility {

VoxelObject::VoxelObject(const std::string path)
{
    try {
        from_qb(path, data_, center_, size_);
        ok_ = true;
    } catch (const std::exception& e) {
        LOG_ERROR(
            logging::file_io_logger, "Could not create VoxelObject: {}", e.what()
        );
        ok_ = false;
    }
}

// This is partially from goxel with GPL license
void
from_qb(
    const std::filesystem::path path, std::vector<uint32_t>& data,
    std::array<int32_t, 3>& center, std::array<uint32_t, 3>& size
)
{
    // static quill::Logger* logger = logging::get_logger("voxel_io");

    LOG_INFO(
        logging::file_io_logger, "Reading voxels from {}.",
        path.string()
    );

    // Read the tiles from the path specified, and save
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        LOG_ERROR(
            logging::file_io_logger,
            "Could not open {}. Are you in the right directory?",
            path.string()
        );
        throw exc::file_not_found_error(path);
    }

    // Read file header
    uint32_t void_; // void int used to read 32 bites without saving it.
    uint32_t compression;

    LOG_TRACE_L1(logging::file_io_logger, "Reading file header");

    //  none of these are used
    read_int(file, void_);       // version
    read_int(file, void_);       // color format RGBA
    read_int(file, void_);       // orientation right handed // c
    read_int(file, compression); // compression
    read_int(file, void_);       // vmask
    read_int(file, void_);       // number of layers

    if (compression) {
        LOG_ERROR(logging::file_io_logger, "Cannot parse voxel files with compression");
        throw exc::not_implemented_error("Cannot parse compressed voxel files");
    }

    // Read file name
    LOG_TRACE_L1(logging::file_io_logger, "Reading voxel save name");

    int8_t name_len;
    read_int(file, name_len);

    LOG_DEBUG(logging::file_io_logger, "Voxel save name length: {}", name_len);

    std::string name(name_len, '\0');
    file.read(name.data(), name_len);

    LOG_INFO(logging::file_io_logger, "Voxel save name: {}", name);

    // Get voxel grid size
    uint32_t x_max, y_max, z_max;

    read_int(file, x_max); // x
    read_int(file, z_max); // z
    read_int(file, y_max); // y

    size = {x_max, y_max, z_max};
    data.resize(x_max * y_max * z_max);

    LOG_DEBUG(
        logging::file_io_logger, "Voxel grid size: {X} x {Y} x {Z}", x_max, y_max, z_max
    );

    // Get voxel grid center
    int32_t x_center, y_center, z_center;

    read_int(file, x_center); // x
    read_int(file, z_center); // z
    read_int(file, y_center); // y

    center = {x_center, y_center, z_center};

    LOG_DEBUG(
        logging::file_io_logger, "Voxel grid center: ({X}, {Y}, {Z})",
        x_center, y_center, z_center
    );

    // Read the voxels themselves
    LOG_DEBUG(logging::file_io_logger, "Reading voxels");

    size_t voxels_read = 0;
    for (size_t x = 0; x < size[0]; x++)
        for (size_t z = 0; z < size[2]; z++)
            for (size_t y = size[1] - 1; y < size[1]; y--) {
                uint32_t raw_color;

                read_int(file, raw_color);
                data[(x * y_max + y) * z_max + z] = parse_color(raw_color);

                voxels_read++;
            }

    LOG_INFO(logging::file_io_logger, "Voxels read: {}", voxels_read);
}

// This is partially from goxel with GPL license
void
to_qb(const std::filesystem::path path, terrain::Terrain ter, bool compression)
{
    // static quill::Logger* logger = logging::get_logger("voxel_io");

    LOG_INFO(
        logging::file_io_logger, "Saving voxels to {}.",
        path.string()
    );

    if (compression) {
        LOG_ERROR(logging::file_io_logger, "Cannot write voxel files with compression");
        throw exc::not_implemented_error("Cannot write compressed voxel files");
    }

    // Saves the tiles in this to the path specified
    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file) {
        LOG_ERROR(
            logging::file_io_logger,
            "Could not open {}. Are you in the right directory?",
            path.string()
        );
        throw exc::file_not_found_error(path);
    }

    std::array<uint32_t, 3> size = ter.get_size();
    std::array<int32_t, 3> offset = ter.get_offset();

    /*
        uint8_t v[4]; */

    // Write header
    uint32_t count = 1; // the number of layers

    LOG_TRACE_L1(logging::file_io_logger, "Writing file header");

    write_int(file, 257); // version
    write_int(file, 0);   // color format RGBA
    write_int(file, 1);   // orientation right handed // c
    write_int(file, 0);   // no compression
    write_int(file, 0);   // vmask
    write_int(file, count);

    // Write file name
    LOG_TRACE_L1(logging::file_io_logger, "Writing file name");

    const std::string name("Main World");
    write_int(file, name.length());
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
                uint32_t raw_color = export_color(ter.get_voxel(x, y, z));

                // Alpha is either 0 or 255
                if (raw_color & 0xff000000)
                    raw_color |= 0xff000000;

                write_int(file, raw_color);

                voxels_written++;
            }

    LOG_INFO(logging::file_io_logger, "Voxels written: {}", voxels_written);
}

} // namespace voxel_utility
