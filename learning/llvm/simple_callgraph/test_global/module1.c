#include <stdio.h>
#include "inline.h"

void global_print(int);
void global_deref();

void *global_fptr1 = (void*)(global_print);
void *global_fptr2 = (void*)(global_deref);
void (*global_fptr3)() = global_deref;

void global_print(int num)
{
  inline_function();
  printf("From global print - num: %d\n", num);
}

void global_deref()
{
  printf("From dereference.\n");
}
