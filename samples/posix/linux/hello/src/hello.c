#include <stdio.h>
#if 0
int main(int argc, char **argv)
{
	return printf("Hello, world!\n");
}
#else
#include <unistd.h>

#ifndef SYS_write
#define SYS_write 64
#endif

#ifndef SYS_exit
#define SYS_exit 93
#endif

long syscall(long num, ...)
{
	long ret;

	__asm__ volatile (
		"mv a7, %0\n"
		"ecall\n"
		: "=r"(ret)
		: "r"(num)
		: "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "memory"
	);

	return ret;
}

void _start() {
    const char msg[] = "Hello World!\n";
    const int len = sizeof(msg) - 1; // Exclude null terminator
    syscall(SYS_write, 1, msg, len); // SYS_write is the syscall number for write
    syscall(SYS_exit, 0); // SYS_exit is the syscall number for exit
}
#endif
