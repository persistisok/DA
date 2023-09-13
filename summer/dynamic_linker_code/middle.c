// example.c
#include <stdio.h>

// 外部函数声明，这个函数定义在另一个共享库中
extern void print_sentence(const char* sentence);

void example_function() {
    // 调用另一个共享库的函数
    print_sentence("Hello from example_function!");
}

