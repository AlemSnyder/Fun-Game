-- Biome map files mush define a function that retunrs a strucutre with a "map".
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
            result["map"][x * number + y] = math.floor( 3 * (noise:sample(x * spacing, y * spacing) + 1) )
        end
    end
    return result
end



--return result
