//
// Created by vsvood on 9/23/21.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <cstddef>

#include "custom_status_lib.h"

struct Stack {
  int* data;
  size_t size;
  size_t capacity;
  static CustomStatus Ctor(Stack* self, size_t capacity);
  static CustomStatus Dtor(Stack* self);
  static CustomStatus Push(Stack* self, int value);
  static CustomStatus Top(Stack* self, int* value);
  static CustomStatus Pop(Stack* self);
  static CustomStatus SmartRealloc(Stack* self);
};

#endif  // STACK_STACK_H
