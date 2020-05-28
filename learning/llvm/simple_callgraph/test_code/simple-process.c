#include <stdio.h>

int sum(int l, int r)
{
  return l + r;
}

int sub(int l, int r)
{
  return l - r;
}

int call_sum(int l, int r)
{
  int (*func)(int, int) = sum;
  //func = &sum;

  return func(l, r);
}

int call_sub(int l, int r)
{
  int (*func)(int, int) = sub;
  //func = &sub;

  return func(l, r);
}

int to_int(char ch)
{
  int lhs=1, rhs=2;
  int k = sum(lhs, rhs);
  printf("In function to_int: sum %d", k);

  int (*s)(int, int);
  int (*p)(int, int) = &sum;
  s=&sum;
  s(3,4);

  return ch;
}

int max(int a, int b)
{
  if (a > b)
    return a;
  else
    return b;
}

int main(int argc, char* argv[])
{
  //int res_sum = sum(1, 2);
  //int res_to_int = to_int('p');

  //printf("Sum is %d. Character ASCII code is %d.\n", res_sum, res_to_int);

  int sum1 = call_sum(1, 2);
  int sub1 = call_sub(2, 1);

  void *k;

  k = &call_sum;

  int flag = 1, b = 0;

  //sum1 = 3;
  //sub1 = 5;

  //printf("%d %d\n", sum1, sub1);

  return 0;
}

