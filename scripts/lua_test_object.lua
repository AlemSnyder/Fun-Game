function takes_object(object)
    Logging.LOG_INFO("object:get_health() = " .. object:get_health())
    object:take_damage(1);
    Logging.LOG_INFO("object:get_health() = " .. object:get_health())

    local position = object:get_position()

    Logging.LOG_INFO("object:get_position() = [" .. position.x .. ", " .. position.y .. ", " .. position.z .. "]")
    return 1
end