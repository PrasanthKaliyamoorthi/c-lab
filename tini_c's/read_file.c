#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int read_file_write(char* file) {
  int fd = open(file, O_RDONLY);

  if (fd < 0)
    return 1;

  struct stat st;
  if (fstat(fd, &st) < 0)
    return 1;

  char* buffer = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (buffer == MAP_FAILED)
    return 1;

  write(1, buffer, st.st_size);

  munmap(buffer, st.st_size);

  close(fd);
  return 0;
}

int main(int argc, char* argv[]) {
  if (read_file_write("read_file.c") > 0)
    return 1;
  return 0;
}
