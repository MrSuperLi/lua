extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
};
#include <stdio.h>
#include <string.h>
#include <cstdlib>



// g++ lua_c.cpp -o lua_c -llua -ldl

void error(lua_State *L, const char *fmt, ...);

int main(void)
{
    char buff[256];
    int err;

    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        err = luaL_loadbuffer(L, buff, strlen(buff), "line") ||
            lua_pcall(L, 0, 0, 0);

        if (err) {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            //error(L, "%s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }


    lua_close(L);
    return 0;
}

void error(lua_State *L, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    vfprintf(stderr, fmt, arg);

    va_end(arg);
    lua_close(L);
    exit(EXIT_FAILURE);
}