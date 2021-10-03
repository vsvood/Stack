#include <cstdio>

#include "stack.h"

int main() {
  Stack stack = {};
  Stack::Dump(&stack);
  Stack::Ctor(&stack, 0, sizeof(int));
  Stack::Dump(&stack);
  int val = 200003;
  Stack::Push(&stack, &val);
  Stack::Dump(&stack);
  val = 2;
  Stack::Push(&stack, &val);
  Stack::Dump(&stack);
  val = static_cast<int>(0xf0f0f0f0);
  Stack::Push(&stack, &val);
  Stack::Dump(&stack);
  val = 4;
  Stack::Push(&stack, &val);
  Stack::Dump(&stack);
  val = 5;
  Stack::Push(&stack, &val);
  Stack::Dump(&stack);

  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);

  Stack::Dump(&stack);

  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);

  Stack::Dump(&stack);

  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);

  Stack::Dump(&stack);

  Stack::Dtor(&stack);
  Stack::Dump(&stack);

  return 0;
}
