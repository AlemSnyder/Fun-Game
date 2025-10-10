Base = Base or {}
Base.entities = Base.entities or {}
Base.entities.Test_Entity = Base.entities.Test_Entity or {}

function Base.entities.Test_Entity.update(this)
    local position = this:get_position()
    if position.z > 60 then
        return vec3(position.x, position.y, 30.0)
    end
    return vec3(position.x, position.y, position.z + 0.01)
end

function Base.entities.Test_Entity.plan(this)
    plan_table = {}
    path = PathFinder:find("Base/entities/Flower_Test")
    if path["len"] > 0 then
        plan_table["follow"] = path
        plan_table["action"] = "destroy"
        return plan_table
    end
    return nil
end

function Base.entities.Test_Entity.take_damage(this, damage)
    this.health = this.health - damage
end

-- ai.lua file needs a number of functions
--   plan a series of paths and actions with a priority
--     need set of actions "pickup", "destroy", "place" etc
--   needs to know when to stop the plan
__ = {}
__.Base = Base
return __
