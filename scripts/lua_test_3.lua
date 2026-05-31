-- Biome map files must define a function that returns a structure with a "map".
-- The map function is called from c++ and reads the x and y lengths. Using this
-- information the "map" key of the returned structure can be used as if it were
-- a 2D array.

-- Anyway, do something like this and it should work as long as there are
-- correctly defined tile types and tile macros.

spacing = 2

function map(number)
	local result = {}
	result.x = number
	result.y = number
	result["map"] = {}

	local noise = AlternativeWorleyNoise:new(64, 0.5, 64)

	for x = 0, number - 1 do
		for y = 0, number - 1 do
			-- sample noise and set a value
			local height = noise:sample(x * spacing, y * spacing)
			-- each value must be integers. math.floor changes doubles to ints

			local height_map_value = 0
			if height <= 0 then
				height_map_value = 1
			else
				height_map_value = 255
			end

			-- assign tile type to index in map
			result["map"][x * number + y] = height_map_value
		end
	end
	return result
end

--return result
