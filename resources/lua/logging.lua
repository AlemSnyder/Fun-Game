Logging = Logging or {}

function Logging.LOG_BACKTRACE(message)
    local info = debug.getinfo(2)
    if info == nil then
        lua_log_backtrace("Invoked from CPP", -1, message)
    else
        local filename = info.source:match("^.+/(.+)$")
        lua_log_backtrace(filename, info.currentline, message)
    end
end

function Logging.LOG_INFO(message)
    local info = debug.getinfo(2)
    if info == nil then
        lua_log_info("Invoked from CPP", -1, message)
    else
        local filename = info.source:match("^.+/(.+)$")
        lua_log_info(filename, info.currentline, message)
    end
end

function Logging.LOG_DEBUG(message)
    local info = debug.getinfo(2)
    if info == nil then
        lua_log_debug("Invoked from CPP", -1, message)
    else
        local filename = info.source:match("^.+/(.+)$")
        lua_log_debug(filename, info.currentline, message)
    end
end

function Logging.LOG_WARNING(message)
    local info = debug.getinfo(2)
    if info == nil then
        lua_log_warning("Invoked from CPP", -1, message)
    else
        local filename = info.source:match("^.+/(.+)$")
        lua_log_warning(filename, info.currentline, message)
    end
end

function Logging.LOG_CRITICAL(message)

    local info = debug.getinfo(2)
    if info == nil then
        lua_log_critical("Invoked from CPP", -1, message)
    else
        local filename = info.source:match("^.+/(.+)$")
        lua_log_critical(filename, info.currentline, message)
    end
end

Logging.LOG_BACKTRACE("Finished lua logging initialization.")

__ = {}
__.Logging = Logging
return __