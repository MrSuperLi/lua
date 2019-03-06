-- 但是对于 自引用 或者 元素之间引用的 这里不能保存。需要变为表达式的方式
function var_export(_var, returnable)
    --print(serialize('f', debug.getinfo(2)))
    local function _var_export(var)
        local var_type = type(var)
        if var_type == 'table' then
            local str = '{'
            for k,v in pairs(var) do
                if type(k) == 'string' then
                    str = str ..'['..string.format("%q", k)..']=' .. _var_export(v) .. ','
                else
                    str = str ..'['..tostring(k)..']=' .. _var_export(v) .. ','
                end
            end
            return str .. '}'
        else
            return basicSerialize(var)
        end
    end

    if returnable then
        return _var_export(_var)
    else
        return print(_var_export(_var))
    end
end

function basicSerialize(_var)
    if type(_var) == 'number' then
        return tostring(_var)
    elseif type(_var) == 'function' or type(_var) == 'table' then
        return 'nil'
    else
        return string.format("%q", _var)
    end
end



-- 文件所在的目录加入到 LUA_PATH，这样设定后就可以使用 require 函数 加载C库了。

local f =  package.loadlib("./source.so", "luaopen_mylib")

print(f)

print('mylib', mylib)

mylib = f()

print('f()', mylib)

print('mylib.dir', mylib.dir);


var_export(mylib.dir('./'))