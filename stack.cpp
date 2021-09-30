//
// Created by vsvood on 9/23/21.
//

#include "stack.h"

#include <cstring>
#include <cstdint>
#include <algorithm>

#include "alchemy_shop.h"

CustomStatus Stack::Ctor(Stack* self, size_t capacity, size_t elem_size) {
  CustomStatus verification_status = Verify(self);
  if (verification_status != CustomStatus::kOk) {
    return verification_status;
  }
  if (capacity == (size_t) DataPoison::kInvalidSize) {
    return CustomStatus::kBadPassedStackCapacity;
  }
  if (elem_size == 0 || elem_size == (size_t)DataPoison::kInvalidSize) {
    return CustomStatus::kBadPassedElemSize;
  }

  self->data = nullptr;
  self->elem_size = elem_size;
  self->capacity = capacity;
  self->size = 0;

  CustomStatus status = SmartRealloc(self, self->size, capacity);
  if (status != CustomStatus::kOk) {
    return status;
  }

  if (self->data != nullptr) {
    memset(self->data, (int)DataPoison::kDeleted, self->elem_size * self->capacity);
  }

  return CustomStatus::kOk;
}

CustomStatus Stack::Dtor(Stack* self) {
  CustomStatus verification_status = Verify(self);
  if (verification_status != CustomStatus::kOk) {
    return verification_status;
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
  CustomStatus verification_status = Verify(self);
  if (verification_status != CustomStatus::kOk) {
    return verification_status;
  }
  if (val_ptr == nullptr) {
    return CustomStatus::kBadValuePtr;
  }

  CustomStatus status = SmartRealloc(self, self->size + 1);
  if (status != CustomStatus::kOk) {
    return status;
  }
  ++self->size;
  memccpy(&self->data[(self->size-1)*self->elem_size], val_ptr, 1, self->elem_size);

  return CustomStatus::kOk;
}

CustomStatus Stack::Top(Stack* self, void* val_ptr) {
  CustomStatus verification_status = Verify(self);
  if (verification_status != CustomStatus::kOk) {
    return verification_status;
  }
  if (val_ptr == nullptr) {
    return CustomStatus::kBadValuePtr;
  }
  if (self->size == 0) {
    return CustomStatus::kBadSelfStackSize;
  }

  memccpy(val_ptr, &self->data[(self->size - 1)*self->elem_size], 1, self->elem_size);

  return CustomStatus::kOk;
}

CustomStatus Stack::Pop(Stack* self) {
  CustomStatus verification_status = Verify(self);
  if (verification_status != CustomStatus::kOk) {
    return verification_status;
  }
  if (self->size == 0) {
    return CustomStatus::kBadSelfStackSize;
  }

  CustomStatus status = SmartRealloc(self, self->size - 1);
  if (status != CustomStatus::kOk) {
    return status;
  }
  --self->size;
  memset(&self->data[self->size * self->elem_size], (int)DataPoison::kDeleted, self->elem_size);

  return CustomStatus::kOk;
}

CustomStatus Stack::SmartRealloc(Stack *self, size_t new_size, size_t preferred_capacity) {
  CustomStatus verification_status = Verify(self);
  if (verification_status != CustomStatus::kOk) {
    return verification_status;
  }

  size_t new_capacity = new_size;
  if (preferred_capacity != 0) {
    new_capacity = std::max(new_size, preferred_capacity);
  } else if (new_size > self->capacity) {
    if (new_size+1 >= SIZE_MAX/2) {
      new_capacity = SIZE_MAX-1;
    } else {  // max deal with zero capacity case
      new_capacity = std::max(new_capacity, self->capacity * 2);
    }
  } else if (new_size < self->capacity / 4) {
    new_capacity = self->capacity / 2;
  } else {
    return CustomStatus::kOk;
  }

  char* tmp = (char*)realloc(self->data, new_capacity * self->elem_size);
  if (tmp == nullptr) {
    return CustomStatus::kAllocError;
  }
  /*if (self->data != nullptr) {  // self->data is freed after reallocation. can`t poison
    memset(self->data, (int) DataPoison::kFreed, self->elem_size * self->capacity);
  }*/
  self->data = tmp;
  self->capacity = new_capacity;
  memset(self->data + self->size*self->elem_size, (int) DataPoison::kDeleted, self->capacity-self->size);

  return CustomStatus::kOk;
}

CustomStatus Stack::Verify(Stack *self) {
  if (self == nullptr || self == (Stack*)PtrPoison::kFreed) {
    return CustomStatus::kBadStackPtr;
  }
  if (self->elem_size == (size_t)DataPoison::kInvalidSize) {
    return CustomStatus::kBadSelfElemSize;
  }
  if (self->size == (size_t)DataPoison::kInvalidSize) {
    return CustomStatus::kBadSelfStackSize;
  }
  if (self->capacity < self->size ||
      self->capacity == (size_t)DataPoison::kInvalidSize) {
    return CustomStatus::kBadSelfStackCapacity;
  }
  if (self->capacity != 0 &&
      (self->data == nullptr || self->data == (char*)PtrPoison::kFreed)) {
    return CustomStatus::kBadSelfDataPtr;
  }
  return CustomStatus::kOk;
}
