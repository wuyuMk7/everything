#include <stdio.h>
#include "inline.h"

extern void *global_fptr1;
extern void *global_fptr2;
extern void (*global_fptr3)();

int main(int argc, char *argv[])
{
  printf("In main now.\n"); 
  inline_function();

  void (*ptr1)(int);
  ptr1 = (void (*)(int))global_fptr1;

  ptr1(20);

  printf("ptr2-gfptr2:\n");
  void (*ptr2)();
  ptr2 = *((void (*)())global_fptr2);
  ptr2();

  printf("ptr3-gfptr2:\n");
  void (*ptr3)() = global_fptr2;
  ptr3();

  printf("gfptr3:\n");
  global_fptr3();

  printf("ptr4-gfptr3:\n");
  void (*ptr4)();
  ptr4 = global_fptr3;
  ptr4();

  return 0;
}
