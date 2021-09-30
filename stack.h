//
// Created by vsvood on 9/23/21.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <cstddef>

#include "custom_status_lib.h"

struct Stack {
  char* data;
  size_t elem_size;
  size_t size;
  size_t capacity;
  static CustomStatus Ctor(Stack *self, size_t capacity, size_t elem_size);
  static CustomStatus Dtor(Stack* self);
  static CustomStatus Push(Stack* self, void* val_ptr);
  static CustomStatus Top(Stack* self, void* val_ptr);
  static CustomStatus Pop(Stack* self);
  static CustomStatus SmartRealloc(Stack *self, size_t new_size, size_t preferred_capacity = 0);
  static CustomStatus Verify(Stack* self);
};

#endif  // STACK_STACK_H
