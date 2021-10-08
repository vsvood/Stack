//
// Created by vsvood on 10/4/21.
//

#include "stack_test.h"

#include <cstdio>
#include <cstring>

void CycleTest();

void SimpleTest();

void StackTest() {
  FILE* log = fopen(STACK_LOG_PATH, "w");
  fclose(log);
  SimpleTest();
  CycleTest();
}

ElemStatus Verify(const void* self);

ElemStatus Dump(const void* self, const char* indent);

void CycleTest() {
  FILE* log = fopen(STACK_LOG_PATH, "a");
  fprintf(log, "############################   CYCLE STACK TEST   ###############################\n");
  fflush(log);
  Stack stack1 = {};
  Stack stack2 = {};
  Stack *stack1_ptr = nullptr;
  Stack *stack2_ptr = nullptr;

  fprintf(log, "Dumping void stack 1\n");
  fflush(log);
  STACK_DUMP(stack1);
  STACK_CTOR(stack1, Stack*);
  fprintf(log, "Dumping Ctored stack 1\n");
  fflush(log);
  STACK_DUMP(stack1);

  stack1.elem_interface = {
    nullptr,
    nullptr,
    Dump,
    Verify
  };
  stack1_ptr = &stack1;

  fprintf(log, "Dumping void stack 2\n");
  fflush(log);
  STACK_DUMP(stack2);
  STACK_CTOR(stack2, Stack*);
  fprintf(log, "Dumping Ctored stack 2\n");
  fflush(log);
  STACK_DUMP(stack2);

  stack2.elem_interface = {
    nullptr,
    nullptr,
    Dump,
    Verify
  };
  stack2_ptr = &stack2;

  fprintf(log, "Pushing stack1 ---> stack2 and stack2 ---> stack1\n");
  fflush(log);
  Stack::Push(&stack1, &stack2_ptr);
  Stack::Push(&stack2, &stack1_ptr);

  fprintf(log, "Dumping cycled stacks\n");
  fflush(log);
  STACK_DUMP(stack1);

  Stack::Dtor(&stack1);
  fprintf(log, "Dumping stack 2 after deleting stack 1\n");
  fflush(log);
  STACK_DUMP(stack2);

  Stack::Dtor(&stack2);
  fprintf(log, "Dumping stack 2 after deleting self\n");
  fflush(log);
  STACK_DUMP(stack2);
  fprintf(log, "############################   CYCLE TEST OK   ###############################\n");
  fflush(log);
  fclose(log);
}

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

void SimpleTest() {
  FILE* log = fopen(STACK_LOG_PATH, "a");
  fprintf(log, "############################   SIMPLE STACK TEST   ###############################\n");
  fflush(log);

  Stack stack = {};

  fprintf(log, "Dumping void stack\n");
  fflush(log);
  STACK_DUMP(stack);
  STACK_CTOR(stack, int);
  fprintf(log, "Dumping Ctored stack 1\n");
  fflush(log);
  STACK_DUMP(stack);

  int val = 1;
  fprintf(log, "Pushing %d\nDumping\n", val);
  fflush(log);
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);

  val = 2;
  fprintf(log, "Pushing %d\nDumping\n", val);
  fflush(log);
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);

  val = (int)0xfdfdfdfd;
  fprintf(log, "Pushing %d\nDumping\n", val);
  fflush(log);
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);

  val = (int)0xf0f0f0f0;
  fprintf(log, "Pushing %d\nDumping\n", val);
  fflush(log);
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);

  val = 5;
  fprintf(log, "Pushing %d\nDumping\n", val);
  fflush(log);
  Stack::Push(&stack, &val);
  STACK_DUMP(stack);


  fprintf(log, "Popping\nDumping\n");
  fflush(log);
  Stack::Pop(&stack);
  STACK_DUMP(stack);


  fprintf(log, "Popping\nDumping\n");
  fflush(log);
  Stack::Pop(&stack);
  STACK_DUMP(stack);


  fprintf(log, "Popping\nDumping\n");
  fflush(log);
  Stack::Pop(&stack);
  STACK_DUMP(stack);


  fprintf(log, "Popping\nDumping\n");
  fflush(log);
  Stack::Pop(&stack);
  STACK_DUMP(stack);


  fprintf(log, "Popping\nDumping\n");
  fflush(log);
  Stack::Pop(&stack);
  STACK_DUMP(stack);

  Stack::Dtor(&stack);
  fprintf(log, "Dumping Dtored stack\n");
  fflush(log);
  STACK_DUMP(stack);
  fprintf(log, "############################   SIMPLE  TEST OK   ###############################\n");
  fflush(log);
  fclose(log);
}
