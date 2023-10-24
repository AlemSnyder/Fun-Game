-- Biome map files must define a function that returns a structure with a "map".
-- The map function is called from c++ and reads the x and y lengths. Using this
-- information the "map" key of the returned structure can be used as if it were
-- a 2D array.

-- Anyway, do something like this and it should work as long as there are
-- correctly defined tile types and tile macros.

spacing = 4

function map(number)
    result = {}
    result.x = number
    result.y = number
    result["map"] = {}

    noise = FractalNoise:new(8, 0.8, 3)

    for x = 0, number-1 do
        for y = 0, number-1 do
            -- sample noise and set a value
            height = 2 * noise:sample(x * spacing, y * spacing)
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



--return result
