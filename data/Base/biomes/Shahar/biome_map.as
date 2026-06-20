// Biome map files must define a class that has a "sample" method. The sample
// method is called from c++ and takes the x, and y coordinates. The method
// should be completely procedural, and except any integer x and y. This might
// be used to do long range rendering so it is important it is well defined.

// Anyway, do something like this and it should work as long as there are
// correctly defined tile types and tile macros.

int NUM_GRASS = 5;

class biome_map {

    TerrainGeneration::FractalNoise noise;
    TerrainGeneration::AlternativeWorleyNoise mountain_noise;
    TerrainGeneration::AlternativeWorleyNoise precipitation_noise;
    TerrainGeneration::AlternativeWorleyNoise flower_noise;
    float spacing;

    biome_map() {
        noise = TerrainGeneration::FractalNoise(4, 0.3, 8);
        mountain_noise = TerrainGeneration::AlternativeWorleyNoise(16.0, 0.25, 8.0);
        precipitation_noise = TerrainGeneration::AlternativeWorleyNoise(12.0, 0.5, 6.0);
        flower_noise = TerrainGeneration::AlternativeWorleyNoise(32, 0.5, 32);
        spacing = 0.4;
    }

    int sample(int x, int y) {
        // sample noise and set a value
        float sea_height = 60.0 * noise.sample(float(x) * spacing, float(y) * spacing) - 11;
        // each value must be integers. math.floor changes doubles to ints
        float mountain_height = 2 * (mountain_noise.sample(float(x + y) * 10 * spacing, float(x - y) * 10 * spacing) - .4) * noise.sample(float(y + 500) * spacing, float(x + 500) * spacing);
        
        if (mountain_height < 0) {
            mountain_height = 0;
        }
        // this makes the oceans shallower
        if (sea_height < 1) {
            sea_height = (sea_height - 1) / 2 + 1;
        }
        float height = sea_height;
        float height_intermediate = sea_height + mountain_height;
        if (height_intermediate < 3) { 
            height = height_intermediate;
        }
        int height_map_value =  int(height);
        // This biome only defines tile types between 0, and 6
        if (height_map_value > 5) {
            height_map_value = 5;
        }
        else if (height_map_value < 0) {
            height_map_value = 0;
        }
        if (height_map_value == 0) {
            return height_map_value;
        } else if (height_map_value >= 4) {
            return height_map_value + 6;
        }

        if (sea_height > 2) {
            sea_height = 2;
        }

        float precipitation = 11 - (precipitation_noise.sample(float(x - y + 900) * spacing / 10, float(x + y - 300) * spacing / 10) * 3 + sea_height * 3);

        if (precipitation < 3){
            return height_map_value + 6;
        } else if (precipitation < 5) {
            return height_map_value + 3;
        } else {
            return height_map_value;
        }
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
