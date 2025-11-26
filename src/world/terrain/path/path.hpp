#pragma once
#include "types.hpp"
#include <vector>

namespace world {

namespace terrain {

namespace path {

class Path {
 private:
    std::vector<TerrainOffset3> path;
    std::string end_object;
};

class PathFinder {
 private:
    std::weak_ptr<const Terrain> ter_;

 public:
    PathFinder(std::shared_ptr<const Terrain> ter);

    Path get_path_A(TerrainOffset3, TerrainOffset3 ) const;

    Path get_path_B(TerrainOffset3, std::set<TerrainOffset3>) const;

    Path get_path_S(TerrainOffset3, std::string) const;

    Path get_path_S(TerrainOffset3, std::set<std::string>) const;

};

}

}

}
