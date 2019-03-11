#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// gcc userdata.c -llua -shared -o array.so
// 有一个很大的安全漏洞:
// array.set(io.stdin, 1, 0)。io.stdin 中的值是一个带有指向流(FILE*)的指针的 userdatum
// 为了区分数组和其他的userdata，我们单独为数组创建了一个metatable
// （记住userdata也可以拥有 metatables）
// 在 Lua 中一般习惯于在 registry 中注册新的 C 类型，使用类型名作为索引，metatable 作为值。

typedef struct NumArray
{
    int size;
    double values[1];
} NumArray;

static NumArray *checkarray(lua_State *L)
{
    void *a = luaL_checkudata(L, 1, "LuaBook.array");

    luaL_argcheck(L, a != NULL, 1,  "'array' expected");

    return (NumArray *)a;
}


static int newarray(lua_State *L)
{
    int n = luaL_checkinteger(L, 1);
    size_t nbytes = sizeof(NumArray) + (n - 1) * sizeof(double);

    NumArray *a = lua_newuserdata(L, nbytes);

    luaL_getmetatable(L, "LuaBook.array");
    lua_setmetatable(L, -2);

    a->size = n;
    return 1;
}

static int setarray(lua_State *L)
{
    NumArray *a = checkarray(L);

    int index = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);

    luaL_argcheck(L, index > 0 && index <= a->size, 2, "index out of rang");

    a->values[index - 1] = value;

    return 0;
}

static int getarray(lua_State *L)
{
    NumArray *a = checkarray(L);

    int index = luaL_checkinteger(L, 2);

    luaL_argcheck(L, index > 0 && index <= a->size, 2, "index out of rang");

    lua_pushnumber(L, a->values[index - 1]);

    return 1;
}

static int getsize(lua_State *L)
{
    NumArray *a = checkarray(L);

    lua_pushnumber(L, a->size);

    return 1;
}

int array2string(lua_State *L)
{
    NumArray *a = checkarray(L);
    lua_pushfstring(L, "array(%d)", a->size);
    return 1;
} 

static const struct luaL_Reg array[] = {
    {"new", newarray},
    {"set", setarray},
    {"get", getarray},
    {"size", getsize},
    {NULL, NULL}
};

static const struct luaL_Reg array_m[] = {
    {"set", setarray},
    {"get", getarray},
    {"size", getsize},
    {"__tostring", array2string},
    {NULL, NULL}
};



int luaopen_array(lua_State *L)
{
    luaL_newmetatable(L, "LuaBook.array");
    
    /* start  访问面向对象的数据*/
    // a = array.new(1000);print(a:size());
    // 给metatable 设置__index
    lua_pushstring("__index"); // key
    lua_pushvalue(L, -2); // copy 的value to the top
    lua_settable(L, -3); // 设置 __index 是自己

    luaL_setfuncs(L, array_m, 0); // 给 metatable 设置 function
    /* end  访问面向对象的数据 */

    lua_newtable(L);
    luaL_setfuncs(L, array, 0);

    return 1;
}
