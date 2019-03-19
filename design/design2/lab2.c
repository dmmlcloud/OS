#include<stdio.h>
#include<sys/syscall.h>
#include<linux/kernel.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    char *src_file = argv[1];
    char *cpy_file = argv[2];
    long a = syscall(333, src_file, cpy_file);
    printf("%ld", a);
    return 0;
}
