//
// Created by vsvood on 9/23/21.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <cstddef>

#include "stack_status.h"
#include "elem_interface.h"

struct Stack {
  size_t capacity;
  size_t size;
  size_t elem_size;
  char* data;
  ElemInterface elem_interface;

  static StackStatus Ctor(Stack *self, size_t capacity, size_t elem_size, ElemInterface interface = {});
  static StackStatus Dtor(Stack* self);
  static StackStatus Push(Stack* self, const void* val_ptr);
  static StackStatus Top(const Stack* self, void* val_ptr);
  static StackStatus Pop(Stack* self);
  static StackStatus SmartRealloc(Stack *self, size_t new_size, size_t preferred_capacity = 0);
  static StackStatus Verify(const Stack* self);
  static StackStatus Dump(const Stack* self, const char* indent = "");
};

#endif  // STACK_STACK_H
