//
// Created by vsvood on 9/23/21.
//

#include "stack.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "alchemy_shop.h"

CustomStatus Stack::Ctor(Stack *self, size_t capacity) {
  if ((self == nullptr) || (capacity == 0)) {
    return CustomStatus::kWrongInputParams;
  }

  int *tmp = (int *)calloc(capacity, sizeof(int));
  if (tmp == nullptr) {
    return CustomStatus::kRuntimeError;
  }
  self->data = tmp;
  self->capacity = capacity;
  self->size = 0;

  return CustomStatus::kOk;
}

CustomStatus Stack::Dtor(Stack *self) {
  if (self == nullptr) {
    return CustomStatus::kWrongInputParams;
  }

  memset(self->data, (int)DataPoison::kFreed, sizeof(int) * self->capacity);
  free(self->data);
  self->data = (int *)PtrPoison::kFreed;

  self->size = (size_t)DataPoison::kInvalidSize;
  self->capacity = (size_t)DataPoison::kInvalidSize;

  return CustomStatus::kOk;
}

CustomStatus Stack::Push(Stack *self, int value) {
  if (self == nullptr) {
    return CustomStatus::kWrongInputParams;
  }

  ++self->size;
  CustomStatus status = SmartRealloc(self);
  if (status != CustomStatus::kOk) {
    --self->size;
    return status;
  }
  self->data[self->size-1] = value;

  return CustomStatus::kOk;
}

CustomStatus Stack::Top(Stack *self, int *value) {
  if ((self == nullptr) || (value == nullptr)) {
    return CustomStatus::kWrongInputParams;
  }
  if (self->size == 0) {
    return CustomStatus::kRuntimeError;
  }

  *value = self->data[self->size - 1];

  return CustomStatus::kOk;
}

CustomStatus Stack::Pop(Stack *self) {
  if (self == nullptr) {
    return CustomStatus::kWrongInputParams;
  }
  if (self->size == 0) {
    return CustomStatus::kRuntimeError;
  }

  --self->size;
  int tmp = self->data[self->size];
  self->data[self->size] = (int)DataPoison::kDeleted;
  CustomStatus status = SmartRealloc(self);
  if (status != CustomStatus::kOk) {
    self->data[self->size] = tmp;
    ++self->size;
    return status;
  }

  return CustomStatus::kOk;
}

CustomStatus Stack::SmartRealloc(Stack *self) {
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

  int *tmp = (int *)realloc(self->data, new_capacity * sizeof(int));
  if (tmp == nullptr) {
    return CustomStatus::kRuntimeError;
  }
  self->data = tmp;
  self->capacity = new_capacity;

  return CustomStatus::kOk;
}
