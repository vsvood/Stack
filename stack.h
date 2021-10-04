//
// Created by vsvood on 9/23/21.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <cstddef>

#include "stack_status.h"
#include "elem_interface.h"

struct ExtraInfo {
    const char* var_name;
    const char* type_name;
    const char* func_name;
    const char* file_name;
    int line;
};

struct Stack {
  char* name;
  char* type;
  char* ctor_at_func;
  char* ctor_in_file;
  int ctor_on_line;
  size_t capacity;
  size_t size;
  size_t elem_size;
  char* data;
  ElemInterface elem_interface;

  mutable bool is_dumping = false;
  mutable bool is_verifying = false;

  static StackStatus
  Ctor(Stack *self, size_t elem_size, ExtraInfo extra_info = {}, size_t capacity = 0, ElemInterface interface = {});
  static StackStatus Dtor(Stack* self);
  static StackStatus Push(Stack* self, const void* val_ptr);
  static StackStatus Top(const Stack* self, void* val_ptr);
  static StackStatus Pop(Stack* self);
  static StackStatus SmartRealloc(Stack *self, size_t new_size, size_t preferred_capacity = 0);
  static StackStatus Verify(const Stack* self);
  static StackStatus Dump(const Stack *self, ExtraInfo extra_info = {}, const char *indent = "");
};

#define STACK_DUMP(self) Stack::Dump(&self, {#self, "", __PRETTY_FUNCTION__, __FILE__, __LINE__})
#define STACK_CTOR(self, type) Stack::Ctor(&self, sizeof(type), {#self, #type,  __PRETTY_FUNCTION__, __FILE__, __LINE__})

#endif  // STACK_STACK_H
