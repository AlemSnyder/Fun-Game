#ifndef __VOXEL_UTILITY_HPP__
#define __VOXEL_UTILITY_HPP__
// TODO comments
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

namespace VoxelUtility{
    int from_qb(const char* path,
     std::vector<uint32_t>& data,
     std::vector<int>& center,
     std::vector<int>& size);

    template <typename T>
    void WRITE(T v, FILE * file){
        fwrite(&v, sizeof(T), 1, file);
    }
    template <typename T>
    void READ(T &v, FILE * file){
        fread(&v, sizeof(T), 1, file);
    }

    inline uint32_t compress_color(uint8_t v[4]){
        return (uint32_t)v[3] | (uint32_t)v[2] << 8 | (uint32_t)v[1] << 16 | (uint32_t)v[0] << 24;
    }

} // namespace VoxelReader

#endif
