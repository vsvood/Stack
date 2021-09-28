//
// Created by vsvood on 9/23/21.
//

#include "stack.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "alchemy_shop.h"

CustomStatus Stack::Ctor(Stack* self, size_t capacity, size_t elem_size) {
  if ((self == nullptr) || (capacity == 0) || (elem_size == 0)) {
    return CustomStatus::kWrongInputParams;
  }

  char* tmp = (char*)calloc(capacity * elem_size, sizeof(char));
  if (tmp == nullptr) {
    return CustomStatus::kRuntimeError;
  }
  self->data = tmp;
  self->elem_size = elem_size;
  self->capacity = capacity;
  self->size = 0;

  return CustomStatus::kOk;
}

CustomStatus Stack::Dtor(Stack* self) {
  if (self == nullptr) {
    return CustomStatus::kWrongInputParams;
  }

  memset(self->data, (int)DataPoison::kFreed, self->elem_size * self->capacity);
  free(self->data);
  self->data = (char*)PtrPoison::kFreed;

  self->size = (size_t)DataPoison::kInvalidSize;
  self->capacity = (size_t)DataPoison::kInvalidSize;
  self->elem_size = (size_t)DataPoison::kInvalidSize;

  return CustomStatus::kOk;
}

CustomStatus Stack::Push(Stack* self, void* val_ptr) {
  if ((self == nullptr) || (val_ptr == nullptr)) {
    return CustomStatus::kWrongInputParams;
  }

  ++self->size;
  CustomStatus status = SmartRealloc(self);
  if (status != CustomStatus::kOk) {
    --self->size;
    return status;
  }
  memccpy(&self->data[(self->size-1)*self->elem_size], val_ptr, 1, self->elem_size);

  return CustomStatus::kOk;
}

CustomStatus Stack::Top(Stack* self, void* val_ptr) {
  if ((self == nullptr) || (val_ptr == nullptr)) {
    return CustomStatus::kWrongInputParams;
  }
  if (self->size == 0) {
    return CustomStatus::kRuntimeError;
  }

  memccpy(val_ptr, &self->data[(self->size - 1)*self->elem_size], 1, self->elem_size);

  return CustomStatus::kOk;
}

CustomStatus Stack::Pop(Stack* self) {
  if (self == nullptr) {
    return CustomStatus::kWrongInputParams;
  }
  if (self->size == 0) {
    return CustomStatus::kRuntimeError;
  }

  --self->size;
  CustomStatus status = SmartRealloc(self);
  if (status != CustomStatus::kOk) {
    ++self->size;
    return status;
  }
  memset(&self->data[self->size * self->elem_size], (int)DataPoison::kDeleted, self->elem_size);

  return CustomStatus::kOk;
}

CustomStatus Stack::SmartRealloc(Stack* self) {
  if (self == nullptr) {
    return CustomStatus::kWrongInputParams;
  }
  if (self->capacity == 0) {
    return CustomStatus::kRuntimeError;
  }

  size_t new_capacity = 0;
  if (self->size > self->capacity) {
    if (self->size == SIZE_MAX) {
      return CustomStatus::kRuntimeError;
    }
    if (self->size+1 >= SIZE_MAX/2) {
      new_capacity = SIZE_MAX-1;
    } else {
      new_capacity = self->capacity * 2;
    }
  } else if (self->size < self->capacity / 4) {
    new_capacity = self->capacity / 2;
  } else {
    return CustomStatus::kOk;
  }

  char* tmp = (char*)realloc(self->data, new_capacity * self->elem_size);
  if (tmp == nullptr) {
    return CustomStatus::kRuntimeError;
  }
  self->data = tmp;
  self->capacity = new_capacity;

  return CustomStatus::kOk;
}
