#include "types.hpp"
#include "world/terrain/chunk.hpp"

#include <cmath>

namespace util {

namespace position {
// ya this is the worst
// TODO literally just rewrite this
inline ChunkPos
chunk_pos_from_vec(glm::vec3 position) {
    int x = std::floor(position.x);
    int y = std::floor(position.y);
    int z = std::floor(position.z);
    if (x < 0) {
        x -= terrain::Chunk::SIZE - 1;
    }
    if (y < 0) {
        y -= terrain::Chunk::SIZE - 1;
    }
    if (z < 0) {
        z -= terrain::Chunk::SIZE - 1;
    }
    ChunkPos chunk_position(
        x / terrain::Chunk::SIZE, y / terrain::Chunk::SIZE, z / terrain::Chunk::SIZE
    );

    return chunk_position;
}

} // namespace position

} // namespace util
