
class biome_map {

    TerrainGeneration::FractalNoise noise_fractal;
    TerrainGeneration::WorleyNoise noise_worley;
    TerrainGeneration::AlternativeWorleyNoise flower_noise;
    float spacing;

    biome_map() {
        noise_fractal = TerrainGeneration::FractalNoise(4, 0.6, 3);
        noise_worley = TerrainGeneration::WorleyNoise(1, 1.0);
        flower_noise = TerrainGeneration::AlternativeWorleyNoise(32, 0.5, 32);
        spacing = 0.05;
    }

    int sample(int x, int y) {
        // sample noise and set a value
        float intermediate = noise_worley.sample(float(x) * spacing, float(y) * spacing);
        float height = 5.0 * intermediate * intermediate + 0.8;
        height = height + noise_fractal.sample(float(x) * 0.6, y * 0.6) * 8 - 4;
        // each value must be integers. math.floor changes doubles to ints
        int height_map_value =  int(height);
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

void do_something() {
    biome_map@ map = biome_map();
}
