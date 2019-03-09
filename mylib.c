#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

static int counter(lua_State *l);

static int counter(lua_State *L)
{
    int number = lua_tonumber(L, lua_upvalueindex(1));

    lua_pushnumber(L, ++number);
    lua_pushvalue(L, -1); // 复制一份，用于函数返回值

    lua_replace(L, lua_upvalueindex(1)); // 把栈顶的值替换到指定的index

    return 1;
}

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

static int l_map(lua_State *L)
{
    int i, n;

    luaL_checktype(L, 1, LUA_TTABLE);

    luaL_checktype(L, 2, LUA_TFUNCTION);

    n = luaL_len(L, 1);

    lua_newtable(L);

    for(i = 1; i <= n; ++i)
    {
        lua_pushvalue(L, 2); // push function
        lua_rawgeti(L, 1, i); // push t[i]
        lua_call(L, 1, 1);  // call function
        lua_rawseti(L, 3, i); // set array
    }

    return 1;
}

static int l_upper(lua_State *L)
{
    size_t l;
    size_t i;

    luaL_Buffer b;

    const char *s = luaL_checklstring(L, 1, &l);

    luaL_buffinit(L, &b);

    for(size_t i = 0; i < l; ++i)
    {
        luaL_addchar(&b, toupper((unsigned char )(s[i])));
    }
    
    luaL_pushresult(&b);

    return 1;
}

// 生成lua闭包
static int l_newCounter(lua_State *L)
{
    lua_pushnumber(L, 0); // 把上值先压入栈
    lua_pushcclosure(L, &counter, 1); // 把闭包压入栈
    return 1;
}

// 捆绑库里面的函数
static const struct luaL_Reg mylib[] = {
    {"dir", l_dir},
    {"map", l_map},
    {"upper", l_upper},
    {"newCounter", l_newCounter},
    {NULL, NULL}
};

int luaopen_mylib(lua_State *L)
{
    // 这种方式返回一个函数，运行后返回才会把库返回
    lua_newtable(L);
    luaL_setfuncs(L, mylib, 0);
    return 1;
}

// void lua_rawgeti (lua_State *L, int index, int key);
// void lua_rawseti (lua_State *L, int index, int key);

//  lua_rawgeti(L,t,key) 等价于 lua_pushnumber(L, key); lua_rawget(L, t);
//  lua_rawseti(L, t, key) 等价于 lua_pushnumber(L, key);lua_insert(L, -2);lua_rawset(L, t);


// void luaL_buffinit (lua_State *L, luaL_Buffer *B);
// 初始化之后，buffer 保留了一份状态 L 的拷贝，因此当我 们调用其他操作 buffer 的函数的时候不需要传递 L
// void luaL_addchar (luaL_Buffer *B, char c);
// void luaL_addlstring (luaL_Buffer *B, const char *s, size_t l);
// void luaL_addstring (luaL_Buffer *B, const char *s);
// void luaL_pushresult (luaL_Buffer *B);