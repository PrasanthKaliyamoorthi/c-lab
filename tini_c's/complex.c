#include <complex.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  double complex z1 = 3.0 + 4.0 * I;
  double complex z2 = 1.0 - 2.0 * I;

  double complex add = z1 + z2;
  double complex sub = z2 - z1;
  double complex mul = z1 * z2;
  double complex div = z2 / z1;

  printf("z1 = %.2f %+.2fi\n", creal(z1), cimag(z1));
  printf("z2 = %.2f %+.2fi\n", creal(z2), cimag(z2));

  printf("add = %.2f %+.2fi\n", creal(add), cimag(add));
  printf("sub = %.2f %+.2fi\n", creal(sub), cimag(sub));
  printf("mul = %.2f %+.2fi\n", creal(mul), cimag(mul));
  printf("div = %.2f %+.2fi\n", creal(div), cimag(div));

  printf("|z1| = %.2f\n", cabs(z1));
  printf("arg(z1) = %.2f radians\n", carg(z1));

  return 0;
}
