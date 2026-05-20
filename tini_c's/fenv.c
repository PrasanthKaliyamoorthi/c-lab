#pragma STDC FENV_ACCESS ON

#include<fenv.h>
#include<math.h>
#include<stdio.h>

void show() {
  if (fetestexcept(FE_DIVBYZERO)) printf("divided by zero\n");
  if (fetestexcept(FE_OVERFLOW)) printf("overflow\n");
  if (fetestexcept(FE_UNDERFLOW)) printf("underflow\n");
  if (fetestexcept(FE_INEXACT)) printf("inexact\n");
  if (fetestexcept(FE_INVALID)) printf("invalid\n");
printf("----------\n");
}

int main(int argc, char* argv[]) {
  fenv_t env;
  fexcept_t flag;

  printf("at start of the program:\n");
  show();
  printf("set hold:\n");
  feholdexcept(&env);
  printf("0.0/0.0: %d\n", 0.0/0.0);
  show();
  printf("1.3/0: %d\n", 1.3/0);
  show();
  printf("overflow: %d\n", 1.7976931348623157e+308);
  show();
  printf("underflow: %d\n", 2.2250738585072014e-308);
  show();
  printf("clear:\n");
  feclearexcept(FE_ALL_EXCEPT);
  show();
  fesetround(FE_TOWARDZERO);
  printf("to 0: 1.7 = %.2f\n", nearbyint(1.7));
  fesetround(FE_TONEAREST);
  printf("to near: 1.7 = %.2f\n", nearbyint(1.7));
  fesetround(FE_UPWARD);
  printf("to +INF: 1.7 = %.2f\n", nearbyint(1.7));
  fesetround(FE_DOWNWARD);
  printf("to -INF: 1.7 = %.2f\n", nearbyint(1.7));
  printf("get round dir: %d\n", fegetround());
  show();
  printf("in exact: 1.0/3.0 = %.20f\n", 1.0/3.0);
  show();
  printf("unset hold:\n");
  feupdateenv(&env);
  show();
  printf("raise div 0:\n");
  feraiseexcept(FE_DIVBYZERO);
  show();
  printf("save div 0 and restore:\n");
  fegetexceptflag(&flag, FE_DIVBYZERO);
  feclearexcept(FE_ALL_EXCEPT);
  fesetexceptflag(&flag, FE_DIVBYZERO);
  show();
  printf("get env : %d\n", fegetenv(&env));
  show();
  printf("clear:\n");
  show();
  feclearexcept(FE_ALL_EXCEPT);
  printf("set env : %d\n", fesetenv(&env));
  show();
  printf("set default: %d\n", feholdexcept(&env));
  show();

  return 0;
}
