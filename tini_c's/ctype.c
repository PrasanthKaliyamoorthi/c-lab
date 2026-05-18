#include<ctype.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
  printf("isupper(a) : %d\n", isupper('a'));
  printf("islower(b) : %d\n", islower('b'));
  printf("isalpha(C) : %d\n", isalpha('C'));
  printf("isdigit(7) : %d\n", isdigit('7'));
  printf("isxdigit(f) : %d\n", isxdigit('f'));
  printf("isspace(\\t) : %d\n", isspace('\t'));
  printf("isgraph(!) : %d\n", isgraph('!'));
  printf("isprint( ) : %d\n", isprint(' '));
  printf("isblank('$') : %d\n", isblank('$'));
  printf("iscntrl(\\0) : %d\n", iscntrl('\0'));
  printf("ispunct(.) : %d\n", ispunct('.'));
  printf("isalnum(3) : %d\n", isalnum('3'));
  return 0;
}
