-- Biome map files must define a function that returns a structure with a "map".
-- The map function is called from c++ and reads the x and y lengths. Using this
-- information the "map" key of the returned structure can be used as if it were
-- a 2D array.

-- Anyway, do something like this and it should work as long as there are
-- correctly defined tile types and tile macros.

spacing = .05


-- Terrain map
function map(number)
    result = {}
    result.x = number
    result.y = number
    result["map"] = {}

    -- num_octaves persistence prime_index
    noise_F = FractalNoise:new(4, 0.6, 3)

    noise_W = WorleyNoise:new(1, 1.0)

    for x = 0, number-1 do
        for y = 0, number-1 do
            -- sample noise and set a value
            height = 5 * noise_W:sample(x * spacing, y * spacing)^2 + 0.8
            height = height + noise_F:sample(x * .6, y * .6) * 8 - 4
            -- each value must be integers. math.floor changes doubles to ints
            height_map_value = math.floor( height )
            -- This biome only defines tile types between 0, and 6
            if height_map_value > 6 then
                height_map_value = 6
            elseif height_map_value < 0 then
                height_map_value = 0
            end
            -- assign tile type to index in map
            result["map"][x * number + y] = height_map_value
        end
    end
    return result
end



-- Maps for trees and bushes
-- name should be used in json file

-- terrain_map should be a result from map
function plants_map(length, terrain_map)
    result = {}
    result.x = length
    result.y = length
    result["map"] = {}
    result["map"]["Trees_1"] = {}
    result["map"]["Flower_1"] = {}

    for x = 0, result.x - 1 do
        for y = 0, result.y - 1 do

            height = terrain_map["map"][math.floor( x / 32 * result.y + y / 32)]

            if (height == 1) then 
                result["map"]["Trees_1"][ math.floor( x * result.y + y)] = 0.10
            else
                result["map"]["Trees_1"][ math.floor( x * result.y + y)] = 0.0
            end
        end
    end

    -- Flower_1
    flower_noise = AlternativeWorleyNoise:new(32, 0.5, 32);

    for x = 0, result.x - 1 do
        for y = 0, result.y - 1 do
            height = flower_noise:sample(x,y)
            if (height > 0) then 
                result["map"]["Flower_1"][ math.floor( x * result.y + y)] = 0.10
            else
                result["map"]["Flower_1"][ math.floor( x * result.y + y)] = 0.0
            end
        end
    end
    return result
end

