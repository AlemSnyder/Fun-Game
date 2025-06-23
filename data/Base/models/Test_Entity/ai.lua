
function move_direction(this)
    -- vec3 = current_position(this)
    if vec3.x < 10 do
        return {1, 0, 0}
    end
    return {0, 0, 0}
end

function plan()
    plan_table
    path = PathFinder:find("base/Flower_Test")
    if path["len"] > 0 then
        plan_table["follow"] = path
        plan_table["action"] = "destroy"
        return plan_table
    end
    return nil
end

-- ai.lua file needs a number of functions
--   plan a series of paths and actions with a priority
--     need set of actions "pickup", "destroy", "place" etc
--   needs to know when to stop the plan