#pragma once

#include "../logging.hpp"
#include "../terrain/terrain.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace voxel_utility {

class VoxelObject {
 private:
    std::vector<uint32_t> data_;
    std::array<int32_t, 3> center_;
    std::array<uint32_t, 3> size_;
    bool ok_;

    quill::Logger* logger = logging::get_logger("voxel_io");

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

void to_qb(const std::string path, terrain::Terrain ter, bool compression = false);

} // namespace voxel_utility
