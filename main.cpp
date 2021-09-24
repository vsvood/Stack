#include <cstdio>

#include "stack.h"

int main() {
  Stack stack = {};
  Stack::Ctor(&stack, 1);
  Stack::Push(&stack, 1);

  int n;
  Stack::Top(&stack, &n);
  printf("%d", n);
  Stack::Pop(&stack);
  Stack::Push(&stack, 1);
  printf("%zu %zu", stack.capacity, stack.size);
  Stack::Dtor(&stack);

  return 0;
}
