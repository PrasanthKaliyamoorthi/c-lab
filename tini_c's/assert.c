#include <assert.h>

int main(int argc, char* argv[]) {
  assert(1 <= 6);
  assert(6==5);
  assert(0==0);
  assert(5<1);
  return 0;
}
