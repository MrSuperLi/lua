#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

// gcc mylib.c -llua -shared -o source.so

// https://zhidao.baidu.com/question/551971637903019772.html

// C 库里面的函数
static int l_dir(lua_State *L)
{
    DIR *dir;
    struct dirent *entry;
    int i;
    const char *path = luaL_checkstring(L, 1);
    /* open directory */
    dir = opendir(path);
    if (dir == NULL)
    {                                       /* error opening the directory? */
        lua_pushnil(L);                     /*returnniland...*/
        lua_pushstring(L, strerror(errno)); /* error message */
        return 2;                           /* number of results */
    }
    /* create result table */
    lua_newtable(L);
    i = 1;
    while ((entry = readdir(dir)) != NULL)
    {
        lua_pushnumber(L, i++);           /* push key */
        lua_pushstring(L, entry->d_name); /* push value */
        lua_settable(L, -3);
    }
    closedir(dir);
    return 1; /* table is already on top */
}

// 捆绑库里面的函数
static const struct luaL_Reg mylib[] = {
    {"dir", l_dir},
    {NULL, NULL}
};

int luaopen_mylib(lua_State *L)
{
    // 这种方式返回一个函数，运行后返回才会把库返回
    lua_newtable(L);
    luaL_setfuncs(L, mylib, 0);
    return 1;
}

