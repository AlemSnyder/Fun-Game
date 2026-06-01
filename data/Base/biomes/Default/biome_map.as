// Biome map files must define a class that has a "sample" method. The sample
// method is called from c++ and takes the x, and y coordinates. The method
// should be completely procedural, and except any integer x and y. This might
// be used to do long range rendering so it is important it is well defined.

// Anyway, do something like this and it should work as long as there are
// correctly defined tile types and tile macros.

int NUM_GRASS = 8;

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
    float sample_plants(const string& in plant_id, int x, int y) {
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
            int flower_height = int(flower_noise.sample(x * 4, y * 4));
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

// TODO need to add glm
int dirt_coloring(int x, int y, int z) {
    return (z + (x / 16 + y / 16) % 2) / 3 % 2 + NUM_GRASS;
}


void do_something() {
    biome_map@ map = biome_map();
}
