-- I dont know that I'm doing

-- I just realized I don't know how to program in Lua.

-- a simple function that takes a rnumber and returns a number by number array
-- of 2s

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
