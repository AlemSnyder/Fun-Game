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
    plan = Plan()
--    plan_table = {}
    terrain = this.get_terrain()
    path_finder = PathFinder(terrain)
    path = path_finder:find("Base/entities/Flower_Test")
    if path["len"] > 0 then
        plan.add_task(path.path)
--        plan_table["follow"] = path
--        plan_table["action"] = "destroy"
        plan.add_task(path.object, "destroy")
        return plan_table
    end
    return nil
end

function xx(this)
    if (this.plan.completed()) then
        -- make new plan
        bf_find = find this.foods();

        PathTask path_task(bf_find.path)

        InteractTask interact_task(bf_find.object)

        -- follow path then do interact
        Plan plan([path_task, interact_task]);
    end
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
