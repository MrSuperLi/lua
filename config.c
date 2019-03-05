#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define MAX_COLOR 255

struct ColorTable {
    char *name;
    unsigned char red, green, blue;
} colortable[] = {
    {"WHITE", MAX_COLOR, MAX_COLOR, MAX_COLOR},
    {"RED", MAX_COLOR, 0, 0},
    {"GREEN", 0, MAX_COLOR, 0},
    {"BLUE", 0, 0, MAX_COLOR},
    {"BLACK", 0, 0, 0}
};

// gcc config.c -o config -llua -ldl

void load(char *filename, int *width, int *height);
void error(lua_State *L, const char *fmt, ...);
int getfield(lua_State *L,int index, char *key);
void setfield(lua_State *L, int index, char *key, int value);
void setcolor(lua_State *L, struct ColorTable *ct);
double f (lua_State *L, double x, double y);
void va_call(lua_State *L,const char *func, const char *sig, ...);
void test_va(lua_State *L,const char *fmt, ...);

void test_va(lua_State *L,const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    printf("test_va arg%d\n", va_arg(arg, int));

    va_end(arg);
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

void setfield(lua_State *L, int index, char *key, int value)
{
    lua_pushstring(L, key);
    lua_pushnumber(L, (double)value);
    lua_settable(L, index > 0 ? index : (index - 2));
}

void load(char *filename, int *width, int *height)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    unsigned char red, green, blue;

    // 应用程序必须在运行用户脚本之前，执行这个循环。
    int i = 0;
    while (colortable[i].name != NULL)
    {
        //setcolor(L, colortable + (i++));
        setcolor(L,  &colortable[i++]);
    }

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

    if (lua_isstring(L, 1))
    {
        const char *name = lua_tostring(L, 1);
        int i = 0;

        while (colortable[i].name != NULL && strcmp(name, colortable[i].name) != 0)
        {
            ++i;
        }

        if (colortable[i].name == NULL)
        {
            error(L, "invalid value for 'background'");
        } else {
            red = colortable[i].red;
            green = colortable[i].green;
            blue = colortable[i].blue;
        }

    } else if (lua_istable(L, 1)) {
        red = getfield(L, 1, "r");
        green = getfield(L, -3, "g");
        blue = getfield(L, -3, "b");
    } else {
        error(L, "invalid value for 'background'");
    }

    printf("r:%d,g:%d,b:%d\n", red, green, blue);

    printf("function f return:%f\n", f(L, 2, 2));

    // 缺点是，参数的类型必须严格 比如d 参数不能位2，必须为2.0
    va_call(L, "f", "dd>i", 2.0, 2.0, &i);

    printf("function f return:%d\n", i);
    
    //test_va(L, "dd", 2, 2);
    lua_close(L);

}

void setcolor(lua_State *L, struct ColorTable *ct)
{
    lua_newtable(L);
    setfield(L, -1, "r", ct->red);
    setfield(L, -1, "g", ct->green);
    setfield(L, -1, "b", ct->blue);

    lua_setglobal(L, ct->name);// 将 table 出栈并将其赋给一个全局变量名。
}

double f (lua_State *L, double x, double y)
{
    lua_getglobal(L, "f"); // 读取函数
    lua_pushnumber(L, x); //  push 第一个参数
    lua_pushnumber(L, y); // push 第二个参数

    /* do the call (2 arguments, 1 result) 第四个参数可以指定一个 错误处理函数 */
    if (lua_pcall(L, 2, 1, 0))
    {
        error(L, "error run function f:%s\n", lua_tostring(L, -1));
    }

    // 对于错误处理函数
    // 使用 lua_pcall 的最后一个参数来指定错误 处理函数，0 代表没有错误处理函数，
    // 也就是说最终的错误信息就是原始的错误信息。 否则，那个参数应该是一个错误函数被加载的时候在栈中的索引，
    // 注意，在这种情况下， 错误处理函数必须要在被调用函数和其参数入栈之前入栈。

    // 对于错误 pcall 返回值
    // 对于一般错误，lua_pcall 返回错误代码 LUA_ERRRUN。
    // 有两种特殊情况，会返回 特殊的错误代码，因为他们从来不会调用错误处理函数


    // pcall压栈返回值
    // 如果函数返回多个结果，第一个结果被第一个入栈，因此如果 有 n 个返回结果，
    // 第一个返回结果在栈中的位置为-n，最后一个返回结果在栈中的位置 为-1。
    // 第一种情况是，内存分配错误， 对于这种错误，lua_pcall 总是返回 LUA_ERRMEM。
    // 第二种情况是，当 Lua 正在运行错误处理函数时发生错误，这种情况下，
    // 再次调用错误处理函数没有意义，所以 lua_pcall立即返回错误代码 LUA_ERRERR。


    if (! lua_isnumber(L, -1))
    {
        error(L, "fucntion 'f' must return number\n");
    }

    double z = lua_tonumber(L, -1);
    lua_pop(L, -1); // 把值出栈

    return z;
}

void va_call(lua_State *L, const char *func, const char *sig, ...)
{
    va_list vl;
    va_start(vl, sig);

    int narg = 0, nres = 0;
    int isBreak = 0;

    //printf("sdd %f\n", va_arg(vl, double));
    //exit(0);

    // 加载函数
    lua_getglobal(L, func);


    while (*sig)
    {
        switch (*sig++)
        {
            case 'd':
                lua_pushnumber(L, va_arg(vl, double));
                break;
            case 'i':
                lua_pushnumber(L, va_arg(vl, int));
                break;
            case 's':
                lua_pushstring(L, va_arg(vl, char *));
                break;
            case '>':
                isBreak = 1;
                break;
            default:
                error(L, "invalid sig type\n");
                break;
        }

        if (isBreak)
        {
            break;
        }

        ++narg;

        // 检测栈空间是否足够
        luaL_checkstack(L, 1, "too many arguments\n");
    }

    // 运行函数
    nres = strlen(sig);
    
    if (lua_pcall(L, narg, nres, 0))
    {
        error(L, "run function error:%s\n", lua_tostring(L, -1));
    }

    // 从栈中获取结果给指针参数
    nres = - nres;
    //error(L, "%d, %f\n", nres, lua_tonumber(L, nres));
    while(*sig)
    {
        switch (*sig++)
        {
            case 'd':
                if (! lua_isnumber(L, nres))
                {
                    error(L, "the %d return value must be number\n", nres);
                }
                *va_arg(vl, double *) = lua_tonumber(L, nres);
                break;
            case 'i':
                if (! lua_isnumber(L, nres))
                {
                    error(L, "the %d return value must be number\n", nres);
                }
                *va_arg(vl, int *) = (int)lua_tonumber(L, nres);
                break;
            case 's':
                if (! lua_isstring(L, nres))
                {
                    error(L, "the %d return value must be string\n", nres);
                }
                *va_arg(vl, const char **) = lua_tostring(L, nres);
                break;
            default:
                error(L, "invalid option (%c)", *(sig - 1));
                break;
        }
        ++nres;
    }
    
    va_end(vl);
}

int main(void)
{
    int width = 0;
    int height = 0;

    load("./config.lua", &width, &height);

    printf("load config,width:%d,height:%d\n", width, height);

    return 0;
}

