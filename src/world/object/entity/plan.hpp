
#include <vector>
#include <variant>
#include <string>
#include <memory>
#include "types.hpp"
#include "object.hpp"
#include "world/terrain/path/path.hpp"

namespace world {

namespace object {

namespace entity {

class PathTask {
 private:
    const std::vector<TerrainOffset3> path_;
    size_t goto_index_; // the task is going to the position of the path with this index
    
 public:

    float get_path_total_length() const;
    float get_path_left_length() const;

    float get_dist_path_goto(ChunkPos chunk_position, LocalFloatPosition local_position) const;
    float get_dist_along_path(ChunkPos chunk_position, LocalFloatPosition local_position) const;

    void update_goto_index();

    PathTask(std::vector<TerrainOffset3> path) : path_(path), goto_index_(0) {}

};

class InteractTask {
 private:
    const std::string interact_object_id_;
    const ChunkPos interact_object_chunk_location_;
    const LocalPosition interact_object_tile_location_;
    const std::string interact_lua_function_;
 public:
    InteractTask(std::shared_ptr<ObjectInstance> object, std::string function_name);

    ChunkPos get_chunk_position();
    LocalPosition get_local_position();

    float dist_from_interact(ChunkPos chunk_position, LocalFloatPosition local_position) const;
};

class Task {
 private:
    std::variant<PathTask, InteractTask> task_;
 public:
    bool can_do_task(std::shared_ptr<ObjectInstance> acting_object) const;
    void do_task(std::shared_ptr<ObjectInstance> acting_object);

    Task(terrain::path::Path path);

    Task(std::shared_ptr<ObjectInstance> object, std::string function_name);
};

class Plan {
 private:
    std::vector<Task> plan_;

 public:
    Plan() {};
    bool completed() const;
    void add_task(Task& task);
    
};

} // namespace entity
    
} // namespace object

} // namespace world
