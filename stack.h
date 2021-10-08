//
// Created by vsvood on 9/23/21.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <cstddef>
#include <cstdio>

#include "stack_status.h"
#include "elem_interface.h"
#include "alchemy_shop.h"
#include "stack_config.h"

struct ExtraInfo {
    const char* var_name;
    const char* type_name;
    const char* func_name;
    const char* file_name;
    int line;
};

struct Stack {
#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  size_t left_canary = (size_t)DataPoison::kCanary;
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
    char left_hash_reference;
#endif  // STACK_SECURE_LEVEL == STACK_SECURE_SELF_HASH

#if STACK_DUMP_LEVEL >= STACK_DUMP_SELF_ALL
  char* name;
  char* type;
  char* ctor_at_func;
  char* ctor_in_file;
  int ctor_on_line;
#endif  // STACK_DUMP_LEVEL >= STACK_DUMP_SELF_ALL

#if STACK_DUMP_LEVEL != STACK_DUMP_NO
  FILE* log_file = fopen(STACK_LOG_PATH, "a");
#endif  // STACK_DUMP_LEVEL != STACK_DUMP_NO

  size_t capacity;
  size_t size;
  size_t elem_size;
  char* data;
  ElemInterface elem_interface;

#if STACK_DUMP_LEVEL & STACK_SECURE_SELF_HASH
    char right_hash_reference;
    uint64_t hash;
#endif  // STACK_DUMP_LEVEL & STACK_SECURE_SELF_HASH

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  size_t right_canary = (size_t)DataPoison::kCanary;
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_DUMP_LEVEL & STACK_DUMP_MEMBERS_MAX
  mutable bool is_dumping = false;
#endif  // STACK_DUMP_LEVEL & STACK_DUMP_MEMBERS_MAX

  mutable bool is_verifying = false;

  static StackStatus Ctor(Stack *self, size_t elem_size, size_t capacity = 0,
                          ElemInterface interface = {},
                          ExtraInfo extra_info = {});
  static StackStatus Dtor(Stack* self);
  static StackStatus Push(Stack* self, const void* val_ptr);
  static StackStatus Top(const Stack* self, void* val_ptr);
  static StackStatus Pop(Stack* self);
  static StackStatus SmartRealloc(Stack *self, size_t new_size, size_t preferred_capacity = 0);
  static StackStatus Verify(const Stack* self);

#if STACK_DUMP_LEVEL > STACK_DUMP_NO
  static StackStatus Dump(const Stack *self, ExtraInfo extra_info = {}, const char *indent = "");
#endif  // STACK_DUMP_LEVEL > STACK_DUMP_NO

};

#if STACK_DUMP_LEVEL > STACK_DUMP_NO
#define STACK_DUMP(self) Stack::Dump(&(self), {#self, "", __PRETTY_FUNCTION__, __FILE__, __LINE__})
#endif  // STACK_DUMP_LEVEL > STACK_DUMP_NO

#define STACK_CTOR(self, type) Stack::Ctor(&(self), sizeof(type), 0, {}, {#self, #type,  __PRETTY_FUNCTION__, __FILE__, __LINE__})

#endif  // STACK_STACK_H
