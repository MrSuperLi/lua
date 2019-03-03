#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COLOR 255

// gcc config.c -o config -llua -ldl

void load(char *filename, int *width, int *height);
void error(lua_State *L, const char *fmt, ...);
int getfield(lua_State *L,int index, char *key);

void error(lua_State *L, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    vfprintf(stderr, fmt, arg);

    va_end(arg);
    lua_close(L);
    exit(EXIT_FAILURE);
}

int getfield(lua_State *L,int index, char *key)
{
    int result;

    lua_pushstring(L, key);

    // lua_gettable的作用就是以栈顶的值作为key来访问-2位置上的table。
    // 并且把key出栈，值放在栈顶
    lua_gettable(L, index > 0 ? index : (index - 1));

    if (!lua_isnumber(L, -1))
    {
        error(L, "invalid component in background color");
    }

    result = (int)lua_tonumber(L, -1);
    lua_pop(L, 1); /* remove number */

    return result;
}

void load(char *filename, int *width, int *height)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
    {
        error(L, "cannot run configuration file: %s\n", lua_tostring(L, -1));
    }

    lua_getglobal(L, "background");
    lua_getglobal(L, "width");
    lua_getglobal(L, "height");

    if (!lua_isnumber(L, -2))
    {
        error(L, "'width' should be a number\n");
    }

    if (!lua_isnumber(L, -1))
    {
        error(L, "'height' should be a number\n");
    }

    // lua_tonumber 函数将得到的数值转换成 double 类型并用(int)强制转换成整型
    *width = (int)lua_tonumber(L, -2);
    *height = (int)lua_tonumber(L, -1);

    printf("r:%d,g:%d,b:%d\n", getfield(L, 1, "r"), getfield(L, -3, "g"), getfield(L, -3, "b"));

    lua_close(L);

}

int main(void)
{
    int width = 0;
    int height = 0;
    load("./config.lua", &width, &height);

    printf("load config,width:%d,height:%d\n", width, height);

    return 0;
}

