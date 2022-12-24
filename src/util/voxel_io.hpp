#pragma once

#include "../logging.hpp"
#include "../exceptions.hpp"
#include "bits.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
//#include <iostream>
#include <string>
#include <vector>

namespace voxel_utility {
// TODO make this file 1k lines long

template <std::integral T>
static inline void
read_int(std::ifstream& file, T& val) noexcept {
    file.read(reinterpret_cast<char*>(&val), sizeof(val));
}

template <std::integral T>
static inline void
write_int(std::ofstream& file, T val) noexcept {
    file.write(reinterpret_cast<char*>(&val), sizeof(val));
}

static inline uint32_t
parse_color(uint32_t color) {
    // Colors are saved in big endian format
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

static inline uint32_t
export_color(uint32_t color) {
    // Colors are saved in big endian format
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

class VoxelLike{
 public:
    uint32_t get_voxel(int x, int y, int z) const;
    std::array<uint32_t, 3> get_size() const;
    std::array<int32_t, 3> get_offset() const;
};

class VoxelObject {
 private:
    std::vector<uint32_t> data_;
    std::array<int32_t, 3> center_;
    std::array<uint32_t, 3> size_;
    bool ok_;

    inline int
    get_position(int x, int y, int z) const
    {
        return ((x * size_[1] + y) * size_[2] + z);
    }

 public:
    /**
     * @brief Construct a new Voxel Object object from saved qb
     *
     * @param path path to .qb file
     */
    VoxelObject(const std::string path);

    /**
     * @brief Construct a new Voxel Object object from saved qb
     *
     * @param path path to .qb file
     */
    VoxelObject(const std::filesystem::path path) : VoxelObject(path.string()) {}

    /**
     * @brief did this voxel object load correctly
     *
     * @return true loaded correctly
     * @return false failed to load correctly
     */
    [[nodiscard]] inline bool
    ok() const noexcept
    {
        return ok_;
    }

    /**
     * @brief Get the voxel color at given coordinate
     *
     * @param x coordinate
     * @param y coordinate
     * @param z coordinate
     * @return uint32_t color
     */
    inline uint32_t
    get_voxel(uint32_t x, uint32_t y, uint32_t z) const
    {
        if ((size_[0] > x) && (size_[1] > y) && (size_[2] > z)) {
            return data_[get_position(x, y, z)];
        }
        return 0;
    }

    /**
     * @brief Get the center of the object
     * use full to find where to rotate around
     *
     * @return std::array<int32_t, 3>
     */
    [[nodiscard]] inline std::array<int32_t, 3>
    get_offset() const noexcept
    {
        return center_;
    }

    /**
     * @brief Get the size as an array of length three
     *
     * @return std::array<uint32_t, 3> length in x, y, z
     */
    [[nodiscard]] inline std::array<uint32_t, 3>
    get_size() noexcept
    {
        return size_;
    }
};

void from_qb(
    const std::filesystem::path path, std::vector<uint32_t>& data,
    std::array<int32_t, 3>& center, std::array<uint32_t, 3>& size
);

template<typename T>
//requires std::is_base_of<VoxelLike, T>::value
void
to_qb(const std::filesystem::path path, T ter, bool compression = false) {
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

    std::array<uint32_t, 3> size = ter.get_size();
    std::array<int32_t, 3> offset = ter.get_offset();

    /*
        uint8_t v[4]; */

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
