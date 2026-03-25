#include "../include/io.h"

int putc(int c, int stream_fd) {
  int ret;
  asm volatile (
    "syscall"
    : "=a"(ret)
    : "a"(1),         // rax: Syscall number 1 (sys_write)
      "D"(stream_fd), // rdi: Arg 1: File descriptor 
      "S"(&c),       // rsi: Arg 2: Pointer to buffer (address of char var c)
      "d"(1)          // rdx: Arg 3: Char count (1 byte)
    : "rcx", "r11", "memory"
  );

}

