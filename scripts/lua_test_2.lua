-- Biome map files must define a function that returns a structure with a "map".
-- The map function is called from c++ and reads the x and y lengths. Using this
-- information the "map" key of the returned structure can be used as if it were
-- a 2D array.

-- Anyway, do something like this and it should work as long as there are
-- correctly defined tile types and tile macros.

function map(number)
    return 5
end



--return result
