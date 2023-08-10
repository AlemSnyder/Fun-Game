-- I dont know that I'm doing

-- I just realized I don't know how to program in Lua.

-- a simple function that takes a rnumber and returns a number by number array
-- of 2s
function map(number)
    map_array = {}
    for x = 0, number-1 do
        for y = 0, number-1 do
            map_array[x * number + y] = 2
        end
    end
    return map_array
end
