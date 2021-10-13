//
// Created by vsvood on 10/8/21.
//

#ifndef STACK_STACK_UTILS_H
#define STACK_STACK_UTILS_H

#include "../include/stack.h"

// General utils

uint64_t CalcHash(const Stack* self);

// Ctor utils

StackStatus CtorCheckParams(Stack *self, size_t elem_size, size_t capacity);

void CtorSetMainInfo(Stack *self, size_t capacity, size_t elem_size);

void CtorPrepareData(Stack* self, StackStatus* status);

// Dtor utils

void DtorClearData(Stack* self, StackStatus* status);

void DtorClearMainInfo(Stack* self);

// Push utils

void PushAddElem(Stack* self, const void *val_ptr, StackStatus* status);

// Top utils

StackStatus TopCheckParams(const Stack *self, void* val_ptr);

void TopGetElem(const Stack* self, void *val_ptr, StackStatus* status);

// Pop utils

StackStatus PopCheckParams(Stack* self);

void PopDelElem(Stack* self, StackStatus* status);

// SmartRealloc utils

size_t SmartReallocNewCapacity(Stack* self, size_t new_size, size_t preferred_capacity);

void SmartReallocRealloc(Stack* self, size_t new_capacity, StackStatus* status);

// Verify utils

void VerifyCheckCtorInfo(const Stack* self, StackStatus* status);

void VerifyCheckMainInfo(const Stack* self, StackStatus* status);

void VerifySelf(const Stack* self, StackStatus* status);

void VerifyCheckSelfCanary(const Stack* self, StackStatus* status);

void VerifyCheckSelfHash(const Stack* self, StackStatus* status);

void VerifySelfSecurity(const Stack* self, StackStatus* status);

void VerifyCheckElems(const Stack* self, StackStatus* status);

// Dump utils

void DumpCycledMsg(const Stack* self, const char* indent, StackStatus status);

void DumpHeader(const Stack* self, CtorInfo ctor_info, const char* indent,  StackStatus status);

void DumpSelfSecurity(const char *indent, StackStatus status);

void DumpSelfInfo(const Stack* self, const char* indent, StackStatus status);

void DumpData(const Stack *self, const char *indent);

#endif //STACK_STACK_UTILS_H
