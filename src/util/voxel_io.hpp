#pragma once

#include <filesystem>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace voxel_utility {

class VoxelObject {
 private:
    std::vector<uint32_t> data_;
    std::vector<int> center_;
    std::vector<uint32_t> size_;
    bool ok_;
    inline int get_position(int x, int y, int z) const {
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
    VoxelObject(const std::filesystem::path path);
    /**
     * @brief did this voxel object load correctly
     * 
     * @return true loaded correctly
     * @return false failed to load correctly
     */
    inline bool ok() const { return ok_; }
    /**
     * @brief Get the voxel color at given coordinate
     * 
     * @param x coordinate
     * @param y coordinate
     * @param z coordinate
     * @return uint32_t color
     */
    inline uint32_t get_voxel(uint32_t x, uint32_t y, uint32_t z) const {
        if ((size_[0] > x) &  // (x >= 0) &
            (size_[1] > y) &  // (y >= 0) &
            (size_[2] > z)) { //& (z >= 0)){

            return data_[get_position(x, y, z)];
        }
        return 0;
    }
    /**
     * @brief Get the center of the object
     * use full to find where to rotate around
     * 
     * @return std::vector<int> 
     */
    inline std::vector<int> get_offset() const { return center_; }
    /**
     * @brief Get the size as a vector of length three
     * 
     * @return std::vector<uint32_t> length in x, y, z
     */
    inline std::vector<uint32_t> get_size() { return size_; }
};

template <typename T>
void
WRITE(T v, FILE* file) {
    fwrite(&v, sizeof(T), 1, file);
}

template <typename T>
void
READ(T& v, FILE* file) {
    fread(&v, sizeof(T), 1, file);
}

int from_qb(
    const std::string path, std::vector<uint32_t>& data, std::vector<int>& center,
    std::vector<uint32_t>& size
);

inline uint32_t
compress_color(uint8_t v[4]) {
    return (uint32_t)v[3] | (uint32_t)v[2] << 8 | (uint32_t)v[1] << 16
           | (uint32_t)v[0] << 24;
}

inline void
export_color(uint32_t tile_color, uint8_t color[4]) {
    color[0] = (tile_color >> 24) & 0xFF;
    color[1] = (tile_color >> 16) & 0xFF;
    color[2] = (tile_color >> 8) & 0xFF;
    color[3] = tile_color & 0xFF;
}

template <typename T>
int
to_qb(const std::string path, T voxel_object) {
    // Saves the tiles in this to the path specified
    FILE* file;
    file = fopen(path.c_str(), "wb");
    if (!file) {
        std::cerr << "Impossible to open " << path
                  << ". Are you in the right directory?" << std::endl;
        getchar();
        fclose(file);
        return -1;
    }

    std::vector<uint32_t> size = voxel_object.get_size();
    std::vector<int> offset = voxel_object.get_offset();

    // This is from goxel with GPL license
    std::cout << "Saving to " << path << "\n";
    std::cout << "    max X: " << size[0] << std::endl;
    std::cout << "    max Y: " << size[1] << std::endl;
    std::cout << "    max Z: " << size[2] << std::endl;
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
    int tiles_written = 0;
    for (x = 0; x < size[0]; x++)
        for (z = 0; z < size[2]; z++)
            for (y = size[1] - 1; y < size[1]; y--) {
                export_color(voxel_object.get_voxel(x, y, z), v);
                if (v[3] != 0x0) {
                    v[3] = 0xFF;
                }
                fwrite(v, 4, 1, file);
                tiles_written++;
            }
    fclose(file);
    std::cout << "    tiles written: " << tiles_written << std::endl;
    return 0;
}

} // namespace voxel_utility
