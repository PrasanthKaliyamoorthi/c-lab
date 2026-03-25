#ifndef IO_H
#define IO_H

#undef putc
#undef puts

int putc(int c, int stream_fd);
int puts(const char* s);
int print(const char* s);

#endif
