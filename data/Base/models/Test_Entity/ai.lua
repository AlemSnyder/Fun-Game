Base = Base or {}
Base.entities = Base.entities or {}
Base.entities.Test_Entity = Base.entities.Test_Entity or {}

function Base.entities.Test_Entity.move_direction(this)
    position = this.get_position();
    if position.x < 10 then
        return {1, 0, 0}
    end
    return {0, 0, 0}
end

function Base.entities.Test_Entity.plan()
    plan_table = {}
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
__ = {}
__.Base = Base
return __
