#include <stdio.h>

// 参考 https://www.cnblogs.com/zhangfeionline/p/5861721.html

extern int a;
extern int b;
extern int add(int, int);
// gcc 1.c 3.c

void main(void)
{
    printf("add = %d\n", a + b);
    printf("function add = %d\n", add(a, b));
}