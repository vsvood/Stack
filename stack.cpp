//
// Created by vsvood on 9/23/21.
//

#include "stack.h"

#include <cstring>
#include <cstdlib>

#include "alchemy_shop.h"
#include "stack_utils.h"

FILE* Stack::log_file = fopen(STACK_LOG_PATH, "a");

StackStatus Stack::Ctor(Stack* const self, size_t elem_size, size_t capacity, CtorInfo ctor_info) {
  StackStatus status = CtorCheckParams(self, elem_size, capacity);
  if (((status & StackStatus::kError) != StackStatus::kOk) ||
      ((status & StackStatus::kWarnSelfVoid) == StackStatus::kOk)) {
    return status;
  }

  CtorSetMainInfo(self, capacity, elem_size);

  self->ctor_info = ctor_info;

  CtorPrepareData(self, &status);

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = CalcHash(self);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  status |= Verify(self);
  return status;
}

StackStatus Stack::Dtor(Stack* self) {
  StackStatus status = Verify(self);
  if ((status & StackStatus::kSelfError) != StackStatus::kOk) {
    return status;
  }

  DtorClearData(self, &status);

  DtorClearMainInfo(self);

  self->ctor_info = {};

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = CalcHash(self);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  return status;
}

StackStatus Stack::Push(Stack* self, const void *val_ptr) {
  StackStatus status = Verify(self);
  if ((status & StackStatus::kError) != StackStatus::kOk) {
    return status;
  }

  PushAddElem(self, val_ptr, &status);

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = CalcHash(self);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  status |= Verify(self);
  return status;
}

StackStatus Stack::Top(const Stack* const self, void* val_ptr) {
  StackStatus status = TopCheckParams(self, val_ptr);
  if ((status & StackStatus::kError) != StackStatus::kOk) {
    return status;
  }

  TopGetElem(self, val_ptr, &status);

  status |= Verify(self);
  return status;
}

StackStatus Stack::Pop(Stack* self) {
  StackStatus status = PopCheckParams(self);
  if ((status & StackStatus::kError) != StackStatus::kOk) {
    return status;
  }

  PopDelElem(self, &status);

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = CalcHash(self);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  status |= Verify(self);
  return status;
}

StackStatus Stack::SmartRealloc(Stack* self, size_t new_size, size_t preferred_capacity) {
  StackStatus status = Verify(self);
  if ((status & StackStatus::kError) != StackStatus::kOk) {
    return status;
  }

  size_t new_capacity = SmartReallocNewCapacity(self, new_size, preferred_capacity);
  if (new_capacity == self->capacity) {
    return status;
  }

  SmartReallocRealloc(self, new_capacity, &status);

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = CalcHash(self);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  status |= Verify(self);
  return status;
}

StackStatus Stack::Verify(const Stack *self) {
  if (self == nullptr) {
    return StackStatus::kBadSelfPtr;
  }
  if (self->is_verifying)
    return StackStatus::kOk;

  self->is_verifying = true;
  StackStatus status = StackStatus::kOk;

  VerifySelf(self, &status);
  VerifySelfSecurity(self, &status);

  if ((status & StackStatus::kSelfError) != StackStatus::kOk) {
    self->is_verifying = false;
    return status;
  }

  VerifyCheckElems(self, &status);

  self->is_verifying = false;
  return status;
}

StackStatus Stack::Dump(const Stack *self, CtorInfo ctor_info, const char *indent) {
  StackStatus status = Stack::Verify(self);

  if ((status & StackStatus::kBadSelfPtr) != StackStatus::kOk) {
    return status;
  }
  if (self->is_dumping) {
    DumpCycledMsg(self, indent, status);
    return StackStatus::kOk;
  }

  self->is_dumping = true;

  DumpSelfSecurity(indent, status);

  if ((status & StackStatus::kBadSelfSecurity) != StackStatus::kOk) {
    return status;
  }

  DumpHeader(self, ctor_info, indent, status);

  DumpSelfInfo(self, indent, status);

  if ((self->capacity != 0) && ((status & StackStatus::kBadSelfSize) == StackStatus::kOk) &&
      ((status & StackStatus::kSelfError) == StackStatus::kOk)) {
    DumpData(self, indent);
  }

  fprintf(Stack::log_file, "\n");
  fflush(Stack::log_file);

  self->is_dumping = false;

  return status;
}
