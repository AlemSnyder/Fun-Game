#ifndef __UTIL_VOXEL_UTILITY_HPP__
#define __UTIL_VOXEL_UTILITY_HPP__
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace VoxelUtility{

    class VoxelObject{
    private:
        std::vector<uint32_t> data_;
        std::vector<int> center_;
        std::vector<uint32_t> size_;
        bool ok_;
    public:
        VoxelObject(const char* path);
        inline bool ok() const {
            return ok_;
        }
        inline int get_position(int x, int y, int z) const {
            return ((x * size_[1] + y) * size_[2] + z);
        }
        inline uint32_t get_voxel(uint32_t x, uint32_t y, uint32_t z){
            if ((size_[0] > x) &// (x >= 0) &
                (size_[1] > y) &// (y >= 0) &
                (size_[2] > z)){//& (z >= 0)){
                
                return data_[get_position(x, y, z)];
            }
            return 0;
        }
        inline std::vector<int> get_offset() const {
            return center_;
        }
        inline std::vector<uint32_t> get_size(){
            return size_;
        }
    };

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
