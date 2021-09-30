#include <cstdio>
#include <cstdint>

#include "stack.h"

int main() {
  Stack stack = {};
  Stack::Ctor(&stack, 0, sizeof(int));
  int val = 200003;
  Stack::Push(&stack, &val);
  val = 2;
  Stack::Push(&stack, &val);
  val = 3;
  Stack::Push(&stack, &val);
  val = 4;
  Stack::Push(&stack, &val);
  val = 5;
  Stack::Push(&stack, &val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Dtor(&stack);

  return 0;
}
