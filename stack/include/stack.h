//
// Created by vsvood on 9/23/21.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H

#include <cstddef>
#include <cstdio>

#include "../src/stack_status.h"
#include "../src/elem_interface.h"
#include "../src/alchemy_shop.h"
#include "../src/stack_config.h"

struct CtorInfo {
    const char* name;
    const char* type;
    const char* func;
    const char* file;
    int line;
};

struct Stack {
#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  size_t left_canary = (size_t)DataPoison::kCanary;
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
    char left_hash_reference;
#endif  // STACK_SECURE_LEVEL == STACK_SECURE_SELF_HASH

  CtorInfo ctor_info = {};

  static FILE* log_file;

  size_t capacity;
  size_t size;
  size_t elem_size;
  char* data;
  ElemInterface elem_interface;

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
    char right_hash_reference;
    uint64_t hash;
#endif  // STACK_DUMP_LEVEL & STACK_SECURE_SELF_HASH

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  size_t right_canary = (size_t)DataPoison::kCanary;
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

  mutable bool is_dumping = false;
  mutable bool is_verifying = false;

  static StackStatus Ctor(Stack *self, size_t elem_size, size_t capacity, CtorInfo extra_info);
  static StackStatus Dtor(Stack* self);
  static StackStatus Push(Stack* self, const void* val_ptr);
  static StackStatus Top(const Stack* self, void* val_ptr);
  static StackStatus Pop(Stack* self);
  static StackStatus SmartRealloc(Stack *self, size_t new_size, size_t preferred_capacity = 0);
  static StackStatus Verify(const Stack* self);
  static StackStatus Dump(const Stack *self, CtorInfo extra_info = {}, const char *indent = "");
};

#define STACK_DUMP(self) Stack::Dump(&(self), {#self, "", __PRETTY_FUNCTION__, __FILE__, __LINE__})

#define STACK_CTOR(self, type, capacity) Stack::Ctor(&(self), sizeof(type), capacity, {#self, #type,  __PRETTY_FUNCTION__, __FILE__, __LINE__})

#endif  // STACK_STACK_H
