/*
 * File: jmp_test.c
 * Author: Yang He (hymeldon@gmail.com)
 * Date: Aug. 17, 2019
 * 
 * This program is used to test function setjmp and longjmp.
 *
 * Works well with gcc 9.1.0 and clang 8.0.1.
 */

#include <stdio.h>
#include <setjmp.h>

void raise_exception(jmp_buf env, int ex)
{
  printf("\n- Raising exception: type %d\n", ex);
  longjmp(env, ex);
}

int main(int argc, char *argv[])
{
  jmp_buf env;

  printf("* Test program: setjmp and longjmp. \n\n");

  int i = setjmp(env);

  printf("- Exception handler: ");
  switch(i) {
  case 0:
    printf("No exception! \n");
    raise_exception(env, 1);
    break; 
  case 1:
    printf("Exception 1! \n");
    raise_exception(env, 2);
    break;
  case 2:
    printf("Exception 2! \n");
    break;

  default:
    break;
  }

  printf("\n* Program exited. \n");

  return 0;
}
