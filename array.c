#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

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



int luaopen_array(lua_State *L)
{
    luaL_newmetatable(L, "LuaBook.array");

    lua_newtable(L);
    luaL_setfuncs(L, array, 0);

    // 设置 __index
    lua_pushstring(L, "__index");
    lua_pushstring(L, "get");

    lua_gettable(L, 2);
    lua_settable(L, 1);

    // 设置 __index
    lua_pushstring(L, "__newindex");
    lua_pushstring(L, "set");

    lua_gettable(L, 2);
    lua_settable(L, 1);


    // 设置 __newindex

    return 1;
}
