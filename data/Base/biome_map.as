// Biome map files must define a function that returns a structure with a "map".
// The map function is called from c++ and reads the x and y lengths. Using this
// information the "map" key of the returned structure can be used as if it were
// a 2D array.

// Anyway, do something like this and it should work as long as there are
// correctly defined tile types and tile macros.

// Base = Base or {}
// Base.biome_map = Base.biome_map or {}

// Base.biome_map.spacing = .8

namespace Base {

namespace biomes {

class biome_map {

    TerrainGeneration::FractalNoise noise; 
    TerrainGeneration::AlternativeWorleyNoise flower_noise;
    float spacing;

    biome_map() {
        noise = TerrainGeneration::FractalNoise(4, 0.6, 3);
        flower_noise = TerrainGeneration::AlternativeWorleyNoise(32, 0.5, 32);
        spacing = 0.8;
    }

    int sample(int x, int y) {
        // sample noise and set a value
        float height = 12.0 * noise.sample(float(x) * spacing, float(y) * spacing) - 4.0;
        // each value must be integers. math.floor changes doubles to ints
        int height_map_value =  int(height) ;
        // This biome only defines tile types between 0, and 6
        if (height_map_value > 6) {
            height_map_value = 6;
        }
        else if (height_map_value < 0) {
            height_map_value = 0;
        }
        return height_map_value;
    }

    // Maps for trees and bushes
    // name should be used in json file
    float sample_plants(string plant_id, int x, int y) {
        if (plant_id == "Trees_1") {
            int height = sample(x, y);
            if (height == 1) {
                return 0.1;
            }
            else {
                return 0.0;
            }
        }
        else if (plant_id == "Flower_1") {
            int flower_height = flower_noise.sample(x * 4, y * 4);
            if (flower_height > 0) {
                return 0.10;
            }
            else {
                return 0.0;
            }
        }
        else {
            return 0.0;
        }
    }
}

}

}

void do_something() {
    Base::biomes::biome_map map = Base::biomes::biome_map();
}
