#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// gcc userdata.c -llua -shared -o array.so

typedef struct NumArray
{
    int size;
    double values[1];
} NumArray;


static int newarray(lua_State *L)
{
    int n = luaL_checkinteger(L, 1);
    size_t nbytes = sizeof(NumArray) + (n - 1) * sizeof(double);

    NumArray *a = lua_newuserdata(L, nbytes);

    a->size = n;
    return 1;
}

static int setarray(lua_State *L)
{
    NumArray *a = (NumArray *) lua_touserdata(L, 1);

    int index = luaL_checkinteger(L, 2);
    double value = luaL_checknumber(L, 3);

    luaL_argcheck(L, a != NULL, 1, "'array' expected");

    luaL_argcheck(L, index > 0 && index <= a->size, 2, "index out of rang");

    a->values[index - 1] = value;

    return 0;
}

static int getarray(lua_State *L)
{
    NumArray *a = (NumArray *) lua_touserdata(L, 1);

    int index = luaL_checkinteger(L, 2);

    luaL_argcheck(L, a != NULL, 1, "'array' expected");

    luaL_argcheck(L, index > 0 && index <= a->size, 2, "index out of rang");

    lua_pushnumber(L, a->values[index - 1]);

    return 1;
}

static int getsize(lua_State *L)
{
    NumArray *a = (NumArray *) lua_touserdata(L, 1);

    luaL_argcheck(L, a != NULL, 1, "'array' expected");

    lua_pushnumber(L, a->size);

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
    lua_newtable(L);
    luaL_setfuncs(L, array, 0);

    return 1;
}
