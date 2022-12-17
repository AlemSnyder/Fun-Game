#pragma once

#include "..\logging.hpp"
#include "bits.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace voxel_utility {

static quill::Logger* logger = logging::get_logger("util.voxel_io");

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

template <typename T>
static inline void
WRITE(T v, FILE* file) noexcept
{
    std::fwrite(&v, sizeof(T), 1, file);
}

template <typename T>
static inline void
READ(T& v, FILE* file) noexcept
{
    std::fread(&v, sizeof(T), 1, file);
}

void from_qb(
    const std::filesystem::path path, std::vector<uint32_t>& data,
    std::array<int32_t, 3>& center, std::array<uint32_t, 3>& size
);

inline uint32_t
parse_color(uint32_t color)
{
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

inline uint32_t
export_color(uint32_t color)
{
    if (std::endian::native == std::endian::little)
        return bits::swap(color);
    else
        return color;
}

template <typename T>
int
to_qb(const std::string path, T voxel_object)
{
    // Saves the tiles in this to the path specified
    FILE* file;
    file = fopen(path.c_str(), "wb");
    if (!file) {
        LOG_ERROR(logger, "Impossible to open {}.", path);
        getchar();
        fclose(file);
        return 1;
    }

    std::vector<uint32_t> size = voxel_object.get_size();
    std::vector<int> offset = voxel_object.get_offset();

    // This is from goxel with GPL license
    LOG_INFO(logger, "Saving to {}", path);
    LOG_DEBUG(logger, "Max X: {}", size[0]);
    LOG_DEBUG(logger, "Max Y: {}", size[1]);
    LOG_DEBUG(logger, "Max Z: {}", size[2]);
    unsigned int count, x, y, z;
    uint8_t v[4];

    count = 1; // the number of layers

    WRITE<uint32_t>(257, file); // version
    WRITE<uint32_t>(0, file);   // color format RGBA
    WRITE<uint32_t>(1, file);   // orientation right handed // c
    WRITE<uint32_t>(0, file);   // no compression
    WRITE<uint32_t>(0, file);   // vmask
    WRITE<uint32_t>(count, file);

    const char* name = "Main World";
    WRITE<int8_t>(std::strlen(name), file);
    fwrite(name, strlen(name), 1, file);
    WRITE<uint32_t>(size[0], file);  // x
    WRITE<uint32_t>(size[2], file);  // z
    WRITE<uint32_t>(size[1], file);  // y
    WRITE<int32_t>(offset[0], file); // x
    WRITE<int32_t>(offset[2], file); // z
    WRITE<int32_t>(offset[1], file); // y
    // iter = mesh_get_accessor(mesh);
    int voxels_written = 0;
    for (x = 0; x < size[0]; x++)
        for (z = 0; z < size[2]; z++)
            for (y = size[1] - 1; y < size[1]; y--) {
                export_color(voxel_object.get_voxel(x, y, z), v);
                if (v[3] != 0x0) {
                    v[3] = 0xFF;
                }
                fwrite(v, 4, 1, file);
                voxels_written++;
            }
    fclose(file);
    LOG_DEBUG(logger, "Voxels written: {}", voxels_written);
    return 0;
}

} // namespace voxel_utility
