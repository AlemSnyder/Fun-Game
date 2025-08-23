
#include "node_group.hpp"
#include "types.hpp"

namespace terrain {

class PositionWrapper {
    TerrainOffset3 position_;

 public:
    inline PositionWrapper(){};

    inline PositionWrapper(TerrainOffset3 position) : position_(position) {}

    [[nodiscard]] glm::vec3
    average_position() const {
        return glm::vec3(position_);
    }

    [[nodiscard]] inline TerrainOffset3
    unique_position() const {
        return position_;
    }

    [[nodiscard]] inline bool
    operator==(const PositionWrapper& other) const {
        return unique_position() == other.unique_position();
    }
};

class NodeGroupWrapper {
    const NodeGroup* nodegroup_;

 public:
    inline NodeGroupWrapper(const NodeGroup* nodegroup) : nodegroup_(nodegroup) {}

    [[nodiscard]] inline glm::vec3
    average_position() const {
        return nodegroup_->sop();
    }

    [[nodiscard]] inline TerrainOffset3
    unique_position() const {
        return nodegroup_->unique_position();
    }

    [[nodiscard]] inline ChunkPos
    get_chunk_position() const {
        return nodegroup_->get_chunk_position();
    }

    [[nodiscard]] inline std::unordered_set<const NodeGroup*>
    get_adjacent_clear(UnitPath path_type) const {
        return nodegroup_->get_adjacent_clear(path_type);
    }

    [[nodiscard]] bool contains(const TerrainOffset3 position) const;

    [[nodiscard]] inline std::unordered_set<TerrainOffset3>
    get_tiles() const {
        return nodegroup_->get_tiles();
    }

    [[nodiscard]] inline bool
    operator==(const NodeGroupWrapper& other) const {
        return unique_position() == other.unique_position();
    }
};

} // namespace terrain

template <>
struct std::hash<terrain::PositionWrapper> {
    inline size_t
    operator()(const terrain::PositionWrapper& position) const noexcept {
        std::hash<TerrainOffset3> hasher;
        return hasher(position.unique_position());
    }
};

template <>
struct std::hash<terrain::NodeGroupWrapper> {
    inline size_t
    operator()(const terrain::NodeGroupWrapper& position) const noexcept {
        std::hash<TerrainOffset3> hasher;
        return hasher(position.unique_position());
    }
};
