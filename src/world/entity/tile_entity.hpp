// has a tile object, but handles other things like dropping items, and interactions

#include "placement.hpp"
#include "tile_object.hpp"

namespace world {

namespace entity {

class TileEntity {
 private:
    TileObject rendered_object_;

 public:
    TileEntity(ModelController model, Position position) :
        rendered_object_(model, position) {}
};

} // namespace entity

} // namespace world
