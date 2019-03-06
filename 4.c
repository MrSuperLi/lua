#include <stdio.h>
#include "1.h"
#include "1.h"
// 参考 https://www.cnblogs.com/zhangfeionline/p/5861721.html



// gcc 1.c 4.c

void main(void)
{
    printf("add = %d\n", a + b);
    printf("4.c function add = %d\n", add(a, b));
}