#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static const char key = 'k';

// registry 是C API中C部分的私有table，只能C库全局共享访问，lua不会直接获得得到
// References 永远不要使用数字作为 registry 的 key，因为这种类型的 key 是保留给 reference 系统使用
// luaL_ref(L, LUA_REGISTRYINDEX); 与 luaL_unref(L, LUA_REGISTRYINDEX, r);

int main(void)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // registry 部分

    // store a number
    lua_pushlightuserdata(L, (void *)&key); // push key
    lua_pushnumber(L, 10);
    lua_settable(L, LUA_REGISTRYINDEX);


    // push a number
    lua_pushlightuserdata(L, (void *)&key);
    lua_gettable(L, LUA_REGISTRYINDEX);

    printf("registry number %d\n", (int)lua_tonumber(L, -1));


    // References 部分
    lua_pushnumber(L, 100);
    int r = luaL_ref(L, LUA_REGISTRYINDEX); // 申请 References

    lua_rawgeti(L, LUA_REGISTRYINDEX, r);

    printf("References %d\n", (int) lua_tonumber(L, -1));

    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_REFNIL); // LUA_NOREF
    printf("References %s\n", lua_tostring(L, -1));

    luaL_unref(L, LUA_REGISTRYINDEX, r); // 释放 References
    
    lua_close(L);

    return 0;
}