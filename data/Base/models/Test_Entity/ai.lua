
function move_direction(this)
    -- vec3 = current_position(this)
    if vec3.x < 10 do
        return (1, 0, 0)
    end
    return (0, 0, 0)
end
