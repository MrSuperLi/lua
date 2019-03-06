#include <stdio.h>
#include "1.c"

// 这样 include 不需要 extern 声明 。include 就类似与替换代码
// 就像是同一个文件

// extern 则不同，它是不需要 include，只要extern声明一下，C编译器自动去找

// gcc 2.c

void main(void)
{
    printf("add = %d\n", a + b);
}