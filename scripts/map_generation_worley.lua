-- Biome map files must define a function that returns a structure with a "map".
-- The map function is called from c++ and reads the x and y lengths. Using this
-- information the "map" key of the returned structure can be used as if it were
-- a 2D array.

-- Anyway, do something like this and it should work as long as there are
-- correctly defined tile types and tile macros.

spacing = 2

function map(number)
    result = {}
    result.x = number
    result.y = number
    result["map"] = {}

    noise = WorleyNoise:new(64, 64)

    for x = 0, number-1 do
        for y = 0, number-1 do
            -- sample noise and set a value
            height = 4 * noise:sample(x * spacing, y * spacing)
            -- each value must be integers. math.floor changes doubles to ints
            height_map_value = math.floor( height * 0.8 )

            -- assign tile type to index in map
            result["map"][x * number + y] = height_map_value
        end
    end
    return result
end



--return result
