#include <cstdio>
#include <cstring>

#include "stack.h"

ElemStatus Verify(const void* self);

ElemStatus Dump(const void* self, const char* indent);

ElemStatus Verify(const void* self) {
  Stack **ptr = new Stack*;
  memccpy(ptr, self, 1, sizeof(Stack*));
  StackStatus status = Stack::Verify(*ptr);
  delete ptr;
  if ((status & StackStatus::kError) != StackStatus::kOk)
    return ElemStatus::kCorrupted;
  if ((status & StackStatus::kWarning) != StackStatus::kOk)
    return ElemStatus::kWarning;
  return ElemStatus::kOk;
}

ElemStatus Dump(const void* self, const char* indent) {
  Stack **ptr = new Stack*;
  memccpy(ptr, self, 1, sizeof(Stack*));
  StackStatus status = Stack::Dump(*ptr, {}, indent);
  delete ptr;
  if ((status & StackStatus::kError) != StackStatus::kOk)
    return ElemStatus::kCorrupted;
  if ((status & StackStatus::kWarning) != StackStatus::kOk)
    return ElemStatus::kWarning;
  return ElemStatus::kOk;
}

int main() {
  Stack stack1 = {};
  Stack stack2 = {};
  Stack* stack1_ptr = nullptr;
  Stack* stack2_ptr = nullptr;

  STACK_DUMP(stack1);
  STACK_CTOR(stack1, Stack*);
  stack1.elem_interface = {
    nullptr,
    nullptr,
    Dump,
    Verify
  };
  stack1_ptr = &stack1;

  STACK_DUMP(stack2);
  STACK_CTOR(stack2, Stack*);
  stack2.elem_interface = {
    nullptr,
    nullptr,
    Dump,
    Verify
  };
  stack2_ptr = &stack2;

  Stack::Push(&stack1, &stack2_ptr);
  Stack::Push(&stack2, &stack1_ptr);

  STACK_DUMP(stack1);

  Stack::Dtor(&stack1);
  STACK_DUMP(stack2);
  Stack::Dtor(&stack2);

  return 0;
}
