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

var_export(mylib.map({1,2,3,4}, function (v) return 2 * v; end));


print(mylib.upper('123abcd'))

-- 生成计数器
counter = mylib.newCounter();
print('计数器:', counter(), counter(), counter());

local array =  package.loadlib("./array.so", "luaopen_array")

print('array', array);

array = array();

print('array', array);

numArray = array.new(5);
print('numArray', numArray);
array.set(numArray, 1, 10)
print('getArray', array.get(numArray, 1));
print('size:', numArray:size());

local array1 =  package.loadlib("./array1.so", "luaopen_array")

array1 = array1();

a = array1.new(5);

a[1] = 12;
print(a[1]);
print('array.size(a)', array.size(a));