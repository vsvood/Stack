//
// Created by vsvood on 9/23/21.
//

#include "stack.h"

#include <cstring>
#include <cmath>
#include <cstdio>

#include "alchemy_shop.h"

StackStatus Stack::Ctor(Stack *self, size_t capacity, size_t elem_size, ElemInterface interface) {
  StackStatus verification_status = Verify(self);
  if (verification_status != StackStatus::kOk) {
    return verification_status;
  }
  if (capacity == (size_t) DataPoison::kInvalidSize) {
    return StackStatus::kBadPassedStackCapacity;
  }
  if (elem_size == 0 || elem_size == (size_t)DataPoison::kInvalidSize) {
    return StackStatus::kBadPassedElemSize;
  }

  self->elem_interface = interface;
  self->data = nullptr;
  self->elem_size = elem_size;
  self->capacity = capacity;
  self->size = 0;

  StackStatus status = SmartRealloc(self, self->size, capacity);
  if (status != StackStatus::kOk) {
    return status;
  }

  if (self->data != nullptr) {
    memset(self->data, (int)DataPoison::kDeleted, self->elem_size * self->capacity);
  }

  return StackStatus::kOk;
}

StackStatus Stack::Dtor(Stack* self) {
//  mb input Stack** and poison?
  StackStatus verification_status = Verify(self);
  if (verification_status != StackStatus::kOk) {
    return verification_status;
  }

  if (self->elem_interface.Dtor != nullptr) {
    for (size_t i = 0; i < self->size; ++i) {
//      TODO add check
      self->elem_interface.Dtor(&self->data[i * self->elem_size]);
    }
  }

  memset(self->data, (int)DataPoison::kFreed, self->elem_size * self->capacity);
  free(self->data);
  self->data = (char*)PtrPoison::kFreed;

  self->size = (size_t)DataPoison::kInvalidSize;
  self->capacity = (size_t)DataPoison::kInvalidSize;
  self->elem_size = (size_t)DataPoison::kInvalidSize;

  return StackStatus::kOk;
}

StackStatus Stack::Push(Stack* self, const void *val_ptr) {
  StackStatus verification_status = Verify(self);
  if (verification_status != StackStatus::kOk) {
    return verification_status;
  }
  if (val_ptr == nullptr) {
    return StackStatus::kBadPassedValuePtr;
  }

  StackStatus status = SmartRealloc(self, self->size + 1);
  if (status != StackStatus::kOk) {
    return status;
  }
  ++self->size;

  if (self->elem_interface.Cpy != nullptr) {
//    TODO add check
    self->elem_interface.Cpy(&self->data[(self->size - 1) * self->elem_size], val_ptr);
  } else {
    memccpy(&self->data[(self->size - 1) * self->elem_size], val_ptr, 1, self->elem_size);
  }

  return StackStatus::kOk;
}

StackStatus Stack::Top(const Stack *self, void* val_ptr) {
  StackStatus verification_status = Verify(self);
  if (verification_status != StackStatus::kOk) {
    return verification_status;
  }
  if (val_ptr == nullptr) {
    return StackStatus::kBadPassedValuePtr;
  }
  if (self->size == 0) {
    return StackStatus::kBadSelfSize;
  }

  if (self->elem_interface.Cpy != nullptr) {
//    TODO add check
    self->elem_interface.Cpy(val_ptr, &self->data[(self->size - 1) * self->elem_size]);
  } else {
    memccpy(val_ptr, &self->data[(self->size - 1)*self->elem_size], 1, self->elem_size);
  }

  return StackStatus::kOk;
}

StackStatus Stack::Pop(Stack* self) {
  StackStatus verification_status = Verify(self);
  if (verification_status != StackStatus::kOk) {
    return verification_status;
  }
  if (self->size == 0) {
    return StackStatus::kBadSelfSize;
  }

  StackStatus status = SmartRealloc(self, self->size - 1);
  if (status != StackStatus::kOk) {
    return status;
  }
  --self->size;

  if (self->elem_interface.Dtor != nullptr) {
//    TODO add check
    self->elem_interface.Dtor(&self->data[self->size * self->elem_size]);
  }
  memset(&self->data[self->size * self->elem_size], (int)DataPoison::kDeleted, self->elem_size);

  return StackStatus::kOk;
}

StackStatus Stack::SmartRealloc(Stack* self, size_t new_size, size_t preferred_capacity) {
  StackStatus verification_status = Verify(self);
  if (verification_status != StackStatus::kOk) {
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
    return StackStatus::kOk;
  }

  char* tmp = (char*)realloc(self->data, new_capacity * self->elem_size);
  if (tmp == nullptr) {
    return StackStatus::kAllocError;
  }
  for (size_t i = self->capacity*self->elem_size; i < new_capacity*self->elem_size; ++i) {
    tmp[i] = (char)DataPoison::kDeleted;
  }
  /*if (self->data != nullptr) {  // self->data is freed after reallocation. can`t poison
    memset(self->data, (int) DataPoison::kFreed, self->elem_size * self->capacity);
  }*/
  self->data = tmp;
  self->capacity = new_capacity;
  memset(self->data + self->size*self->elem_size, (int) DataPoison::kDeleted, self->capacity-self->size);

  return StackStatus::kOk;
}

StackStatus Stack::Verify(const Stack *self) {
  if (self == nullptr || self == (Stack*)PtrPoison::kFreed) {
    return StackStatus::kBadSelfPtr;
  }
  StackStatus status = StackStatus::kOk;
  if (self->capacity == (size_t)DataPoison::kInvalidSize) {
    status |= StackStatus::kBadSelfCapacity;
  }
  if (self->capacity < self->size ||
      self->size == (size_t)DataPoison::kInvalidSize) {
    status |= StackStatus::kBadSelfSize;
  }
  if ((self->capacity != 0 && self->elem_size == 0) ||
      self->elem_size == (size_t)DataPoison::kInvalidSize) {
    status |= StackStatus::kBadSelfElemSize;
  }
  if (self->capacity != 0 &&
      (self->data == nullptr || self->data == (char*)PtrPoison::kFreed)) {
    status |= StackStatus::kBadSelfDataPtr;
  }
  if (status != StackStatus::kOk) return status;

  if (self->elem_interface.Verify != nullptr) {
    for (size_t i = 0; i < self->size; ++i) {
      ElemStatus elem_status = self->elem_interface.Verify(&self->data[i*self->elem_size]);
      if (elem_status != ElemStatus::kOk) {
        return StackStatus::kBadElem;
      }
    }
  }
  return StackStatus::kOk;
}

StackStatus Stack::Dump(const Stack *self, const char *indent) {
  StackStatus status = Stack::Verify(self);
  printf("%s-stack<> [%p] %s\n", indent, self,
         ((status & StackStatus::kBadSelfPtr) == StackStatus::kOk) ? "ok" : "ERROR");
  fflush(stdout);
  if ((status & StackStatus::kBadSelfPtr) == StackStatus::kOk)
  {
    printf("%s  capacity  - %zu %s", indent, self->capacity,
           ((status & StackStatus::kBadSelfCapacity) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(stdout);
    printf("%s  size      - %zu %s", indent, self->size,
           ((status & StackStatus::kBadSelfSize) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(stdout);
    printf("%s  elem_size - %zu %s", indent, self->elem_size,
           ((status & StackStatus::kBadSelfElemSize) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(stdout);
    printf("%s  data      - [%p] %s", indent, self->data,
           ((status & StackStatus::kBadSelfDataPtr) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(stdout);
  }
  if (self->size != 0) {
    if ((status & StackStatus::kBadSelfDataPtr) == StackStatus::kOk) {
      printf("%s  {\n", indent);

      char *poisoned = (char *) calloc(1, self->elem_size+1);
      poisoned[self->elem_size] = '\0';
      char *elem_value = (char *) calloc(1, self->elem_size + 1);
      poisoned[self->elem_size] = '\0';

      for (size_t i = 0; i < self->size; ++i) {
        printf("%s   *[%zu] = ", indent, i);

        memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);
        for (size_t j = self->elem_size; j > 0; --j) {
          printf("%02hhX", elem_value[j - 1]);
        }
        fflush(stdout);

        memset(poisoned, (int) DataPoison::kDeleted, self->elem_size);
        if (strcmp(elem_value, poisoned) == 0) {
          printf("  WARNDelPoison");
        }
        fflush(stdout);

        memset(poisoned, (int) DataPoison::kFreed, self->elem_size);
        if (strcmp(elem_value, poisoned) == 0) {
          printf("  WARNFreePoison");
        }
        fflush(stdout);

        if (self->elem_interface.Verify != nullptr) {
          ElemStatus elem_status = self->elem_interface.Verify(&self->data[i]);
          if (elem_status == ElemStatus::kCorrupted)
            printf("  ERRORCorrupted\n");
          else
            printf("  ok\n");
          if (self->elem_interface.Dump != nullptr) {
            char *new_indent = (char *) calloc(1, strlen(indent) + 6);
            memccpy(new_indent, indent, 1, strlen(indent));
            self->elem_interface.Dump(&self->data[i], strcat(new_indent, "      "));
          }
          fflush(stdout);
        } else {
          printf("\n");
        }
      }
      for (size_t i = self->size; i < self->capacity; ++i) {
        printf("%s    [%zu] = ", indent, i);

        memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);
        for (size_t j = self->elem_size; j > 0; --j) {
          printf("%02hhX", elem_value[j - 1]);
        }
        fflush(stdout);

        memset(poisoned, (int) DataPoison::kDeleted, self->elem_size);
        if (strcmp(elem_value, poisoned) == 0) {
          printf("  okPoisoned\n");
          fflush(stdout);
        } else {
          memset(poisoned, (int) DataPoison::kFreed, self->elem_size);
          if (strcmp(elem_value, poisoned) == 0) {
            printf("  ERRORWrongPoison\n");
            fflush(stdout);
          } else {
            printf("  ERRORNoPoison\n");
            fflush(stdout);
          }
        }
      }

      free(poisoned);
      free(elem_value);

      printf("%s  }\n", indent);
    }
  }
  printf("\n");
  return status;
}
