#include <cstdio>

#include "stack.h"

int main() {
  Stack stack = {};
  STACK_DUMP(stack);
  STACK_CTOR(stack, int);
  STACK_DUMP(stack);
  int val = 200003;
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);
  val = 2;
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);
  val = static_cast<int>(0xf0f0f0f0);
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);
  val = 4;
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);
  val = 5;
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);

  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);

  STACK_DUMP(stack);

  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);

  STACK_DUMP(stack);

  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);
  Stack::Top(&stack, &val);
  Stack::Pop(&stack);
  printf("%d\n", val);

  STACK_DUMP(stack);

  Stack::Dtor(&stack);
  STACK_DUMP(stack);

  Stack::Dtor(&stack);
  STACK_DUMP(stack);

  return 0;
}
