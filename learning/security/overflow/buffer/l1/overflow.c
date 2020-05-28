#include <stdio.h>
#include <string.h>

void func2()
{
  printf("func2\n");
}

void func1(int a)
{
  char buffer[32];

  printf("Please input string:\n");
  scanf("%s", buffer);

  printf("In function, recieved input %s\n", buffer);
}

int main(int argc, char* argv[])
{
  func1(1);
  printf("Back to main.\n"); 

  return 0;
}
