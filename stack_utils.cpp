//
// Created by vsvood on 10/8/21.
//

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "stack_utils.h"

// General utils

uint64_t CalcHash(const Stack *self) {
  uint64_t cur_hash = 0;
  for (const char *elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
    cur_hash ^= ((uint64_t) *elem_p) << ((uint64_t) elem_p % 56);
  }
  return cur_hash;
}

// Ctor utils

StackStatus CtorCheckParams(Stack *self, size_t elem_size, size_t capacity) {
  StackStatus status = Stack::Verify(self);
  if (capacity >= (size_t) DataPoison::kInvalidSize) {
    status |= StackStatus::kBadPassedStackCapacity;
  }
  if (elem_size == 0 || elem_size >= (size_t)DataPoison::kInvalidSize) {
    status |= StackStatus::kBadPassedElemSize;
  }
  return status;
}

void CtorSetMainInfo(Stack *self, size_t capacity, size_t elem_size) {
  self->capacity = capacity;
  self->size = 0;
  self->elem_size = elem_size;
  self->data = nullptr;
}

void CtorPrepareData(Stack *self, StackStatus *status) {
  *status |= Stack::SmartRealloc(self, self->size, self->capacity);
  if (*status != StackStatus::kOk) {
    return;
  }

  if (self->data != nullptr) {
    memset(self->data, (int)DataPoison::kDelete, self->elem_size * self->capacity);
  }
}

// Dtor utils

void DtorClearData(Stack *self, StackStatus *status) {
  if (self->elem_interface.Dtor != nullptr) {
    for (size_t i = 0; i < self->size; ++i) {
      ElemStatus elem_status = self->elem_interface.Dtor(&self->data[i * self->elem_size]);
      if (elem_status == ElemStatus::kCorrupted) {
        *status |= StackStatus::kBadElemDel;
      }
    }
  }

  memset(self->data, (int)DataPoison::kFree, self->elem_size * self->capacity);
  free(self->data);
  self->data = (char*)PtrPoison::kFreed;
}

void DtorClearMainInfo(Stack *self) {
  self->size = (size_t)DataPoison::kInvalidSize;
  self->capacity = (size_t)DataPoison::kInvalidSize;
  self->elem_size = (size_t)DataPoison::kInvalidSize;
}

// Push utils

void PushAddElem(Stack* self, const void *val_ptr, StackStatus *status) {
  *status |= Stack::SmartRealloc(self, self->size + 1);
  if ((*status & StackStatus::kError) != StackStatus::kOk) {
    return;
  }
  ++self->size;

  if (self->elem_interface.Cpy != nullptr) {
    ElemStatus elem_status = self->elem_interface.Cpy(&self->data[(self->size - 1) * self->elem_size], val_ptr);
    if (elem_status == ElemStatus::kCorrupted) {
      *status |= StackStatus::kBadElemCpy;
    }
  } else {
    memccpy(&self->data[(self->size - 1) * self->elem_size], val_ptr, 1, self->elem_size);
  }
}

// Top utils

StackStatus TopCheckParams(const Stack *self, void *val_ptr) {
  StackStatus status = Stack::Verify(self);
  if (val_ptr == nullptr) {
    status |= StackStatus::kBadPassedValuePtr;
  }
  if ((status & StackStatus::kBadSelfPtr) != StackStatus::kOk) {
    return status;
  }
  if (self->size == 0) {
    status |= StackStatus::kBadSelfSize;
  }
  return status;
}

void TopGetElem(const Stack *self, void *val_ptr, StackStatus *status) {
  if (self->elem_interface.Cpy != nullptr) {
    ElemStatus elem_status = self->elem_interface.Cpy(val_ptr, &self->data[(self->size - 1) * self->elem_size]);
    if (elem_status == ElemStatus::kCorrupted) {
      *status |= StackStatus::kBadElemCpy;
    }
  } else {
    memccpy(val_ptr, &self->data[(self->size - 1)*self->elem_size], 1, self->elem_size);
  }
}

// Pop utils

StackStatus PopCheckParams(Stack *self) {
  StackStatus status = Stack::Verify(self);
  if ((status & StackStatus::kBadSelfPtr) != StackStatus::kOk) {
    return status;
  }
  if (self->size == 0) {
    status |= StackStatus::kBadSelfSize;
  }
  return status;
}

void PopDelElem(Stack *self, StackStatus *status) {
  *status |= Stack::SmartRealloc(self, self->size - 1);
  if ((*status & StackStatus::kError) != StackStatus::kOk) {
    return;
  }
  --self->size;

  if (self->elem_interface.Dtor != nullptr) {
    ElemStatus elem_status = self->elem_interface.Dtor(&self->data[self->size * self->elem_size]);
    if (elem_status == ElemStatus::kCorrupted) {
      *status |= StackStatus::kBadElemDel;
    }
  }
  memset(&self->data[self->size * self->elem_size], (int)DataPoison::kDelete, self->elem_size);
}

// SmartRealloc utils

size_t SmartReallocNewCapacity(Stack *self, size_t new_size, size_t preferred_capacity) {
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
    return self->capacity;
  }

  return new_capacity;
}

void SmartReallocRealloc(Stack *self, size_t new_capacity, StackStatus *status) {
  char* tmp = (char*)realloc(self->data, new_capacity * self->elem_size);
  if (tmp == nullptr) {
    *status |= StackStatus::kAllocError;
    return;
  }
  for (size_t i = self->capacity*self->elem_size; i < new_capacity*self->elem_size; ++i) {
    tmp[i] = (char)DataPoison::kDelete;
  }
  self->data = tmp;
  self->capacity = new_capacity;
}

// Verify utils

void VerifyCheckCtorInfo(const Stack *self, StackStatus *status) {
  if ((self->ctor_info.name == nullptr) || (self->ctor_info.type == nullptr) ||
      (self->ctor_info.file == nullptr) || (self->ctor_info.func == nullptr) ||
      (self->ctor_info.line == 0)) {
    *status |= StackStatus::kWarnSelfCtorInfo;
  }
}

void VerifyCheckMainInfo(const Stack *self, StackStatus *status) {
  if ((self->capacity == 0) && (self->size == 0) &&
      (self->elem_size == 0) && (self->data == nullptr)) {
    *status |= StackStatus::kWarnSelfVoid;
  }

  if (self->capacity == (size_t)DataPoison::kInvalidSize)
    *status |= StackStatus::kBadSelfCapacity;
  if (self->capacity < self->size ||
      self->size == (size_t)DataPoison::kInvalidSize)
    *status |= StackStatus::kBadSelfSize;
  if ((self->capacity != 0 && self->elem_size == 0) ||
      self->elem_size == (size_t)DataPoison::kInvalidSize)
    *status |= StackStatus::kBadSelfElemSize;
  if (self->capacity != 0 &&
      (self->data == nullptr || self->data == (char*)PtrPoison::kFreed))
    *status |= StackStatus::kBadSelfDataPtr;
}

void VerifySelf(const Stack *self, StackStatus *status) {
  VerifyCheckCtorInfo(self, status);
  VerifyCheckMainInfo(self, status);
}

void VerifyCheckSelfCanary(const Stack *self, StackStatus *status) {
  if (self->left_canary != (size_t)DataPoison::kCanary)
    *status |= StackStatus::kBadSelfLeftCanary;
  if (self->right_canary != (size_t)DataPoison::kCanary)
    *status |= StackStatus::kBadSelfRightCanary;
}

void VerifyCheckSelfHash(const Stack *self, StackStatus *status) {
  if (self->data) {
    uint64_t cur_hash = CalcHash(self);
    if (cur_hash != self->hash) {
      *status |= StackStatus::kBadSelfHash;
    }
  }
}

void VerifyCheckElems(const Stack *self, StackStatus *status) {
  if (self->elem_interface.Verify != nullptr) {
    for (size_t i = 0; i < self->size; ++i) {
      ElemStatus elem_status = self->elem_interface.Verify(&self->data[i * self->elem_size]);
      if (elem_status == ElemStatus::kCorrupted)
        *status |= StackStatus::kBadElem;
      else if (elem_status == ElemStatus::kWarning)
        *status |= StackStatus::kWarnElemVerify;
    }
  }

  char *poisoned = (char *) calloc(1, self->elem_size + 1);
  poisoned[self->elem_size] = '\0';
  char *elem_value = (char *) calloc(1, self->elem_size + 1);
  poisoned[self->elem_size] = '\0';

  for (size_t i = 0; i < self->size; ++i) {
    memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);

    memset(poisoned, (int) DataPoison::kDelete, self->elem_size);
    if (strcmp(elem_value, poisoned) == 0) {
      *status |= StackStatus::kWarnElemPoisonDelete;
    }
    fflush(Stack::log_file);

    memset(poisoned, (int) DataPoison::kFree, self->elem_size);
    if (strcmp(elem_value, poisoned) == 0) {
      *status |= StackStatus::kWarnElemPoisonFree;
    }
  }

  for (size_t i = self->size; i < self->capacity; ++i) {
    memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);

    memset(poisoned, (int) DataPoison::kDelete, self->elem_size);
    if (strcmp(elem_value, poisoned) != 0) {
      *status |= StackStatus::kBadElemPoison;
    }
  }

  free(poisoned);
  free(elem_value);

}

void VerifySelfSecurity(const Stack *self, StackStatus *status) {
#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  VerifyCheckSelfCanary(self, status);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  VerifyCheckSelfHash(self, status);
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
}

void DumpCycledMsg(const Stack *self, const char *indent, StackStatus status) {
  fprintf(Stack::log_file, "%s cycles to -stack<%s> [%p] ok\n", indent,
          ((status & StackStatus::kWarnSelfCtorInfo) == StackStatus::kOk)
          ? self->ctor_info.type : "", self);
  fflush(Stack::log_file);
}

void DumpHeader(const Stack *self, CtorInfo ctor_info, const char *indent, StackStatus status) {
  fprintf(Stack::log_file, "%s-stack<%s> [%p] %s", indent,
          ((status & StackStatus::kWarnSelfCtorInfo) == StackStatus::kOk)
          ? self->ctor_info.type : "", self,
          ((status & StackStatus::kError) == StackStatus::kOk)
          ? "ok" : "ERROR");

  if (ctor_info.name != nullptr) {
    fprintf(Stack::log_file, " \"%s\" at %s at %s(%d)",
            ctor_info.name, ctor_info.func,
            ctor_info.file, ctor_info.line);
  }
  fprintf(Stack::log_file, "\n");
  fflush(Stack::log_file);
}

void DumpSelfSecurity(const char *indent, StackStatus status) {
#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  if ((status & StackStatus::kBadSelfLeftCanary) != StackStatus::kOk) {
    fprintf(Stack::log_file, "%s  ERROR self left canary damaged! stack status (%d)\n", indent, (int)status);
    fflush(Stack::log_file);
  }
  if ((status & StackStatus::kBadSelfRightCanary) != StackStatus::kOk) {
    fprintf(Stack::log_file, "%s  ERROR self right canary damaged! stack status (%d)\n", indent, (int)status);
    fflush(Stack::log_file);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  if ((status & StackStatus::kBadSelfHash) != StackStatus::kOk) {
    fprintf(Stack::log_file, "%s  ERROR self hash differs! stack status (%d)\n", indent, (int)status);
    fflush(Stack::log_file);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
}

void DumpSelfInfo(const Stack *self, const char *indent, StackStatus status) {
  if ((status & StackStatus::kWarnSelfCtorInfo) == StackStatus::kOk) {
    fprintf(Stack::log_file, "%s  Ctor as \"%s\" at %s in %s(%d)\n",
            indent, self->ctor_info.name, self->ctor_info.func,
            self->ctor_info.file, self->ctor_info.line);
    fflush(Stack::log_file);
  }
  fprintf(Stack::log_file, "%s  capacity  - %zu %s", indent, self->capacity,
          ((status & StackStatus::kBadSelfCapacity) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
  fflush(Stack::log_file);
  fprintf(Stack::log_file, "%s  size      - %zu %s", indent, self->size,
          ((status & StackStatus::kBadSelfSize) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
  fflush(Stack::log_file);
  fprintf(Stack::log_file, "%s  elem_size - %zu %s", indent, self->elem_size,
          ((status & StackStatus::kBadSelfElemSize) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
  fflush(Stack::log_file);
  fprintf(Stack::log_file, "%s  data      - [%p] %s", indent, self->data,
          ((status & StackStatus::kBadSelfDataPtr) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
  fflush(Stack::log_file);
}

void DumpData(const Stack *self, const char *indent) {
  fprintf(Stack::log_file, "%s  {\n", indent);

  char *poisoned = (char *) calloc(1, self->elem_size + 1);
  poisoned[self->elem_size] = '\0';
  char *elem_value = (char *) calloc(1, self->elem_size + 1);
  poisoned[self->elem_size] = '\0';

  char *new_indent = (char *) calloc(1, strlen(indent) + 7);
  memccpy(new_indent, indent, 1, strlen(indent));
  new_indent = strcat(new_indent, "      ");

  for (size_t i = 0; i < self->size; ++i) {
    fprintf(Stack::log_file, "%s   *[%zu] = ", indent, i);

    memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);
    for (size_t j = self->elem_size; j > 0; --j) {
      fprintf(Stack::log_file, "%02hhX", elem_value[j - 1]);
    }
    fflush(Stack::log_file);

    memset(poisoned, (int) DataPoison::kDelete, self->elem_size);
    if (strcmp(elem_value, poisoned) == 0) {
      fprintf(Stack::log_file, "  WARNDelPoison");
    }
    fflush(Stack::log_file);

    memset(poisoned, (int) DataPoison::kFree, self->elem_size);
    if (strcmp(elem_value, poisoned) == 0) {
      fprintf(Stack::log_file, "  WARNFreePoison");
    }
    fflush(Stack::log_file);

    if (self->elem_interface.Verify != nullptr) {
      ElemStatus elem_status = self->elem_interface.Verify(&self->data[i]);
      if (elem_status == ElemStatus::kCorrupted) {
        fprintf(Stack::log_file, "  ERRORCorrupted\n");
        fflush(Stack::log_file);
      } else {
        fprintf(Stack::log_file, "  ok\n");
        fflush(Stack::log_file);
      }
      if (self->elem_interface.Dump != nullptr) {
        self->elem_interface.Dump(&self->data[i], new_indent);
      }
      fflush(Stack::log_file);
    } else {
      fprintf(Stack::log_file, "\n");
    }
    fflush(Stack::log_file);
  }
  free(new_indent);
  for (size_t i = self->size; i < self->capacity; ++i) {
    fprintf(Stack::log_file, "%s    [%zu] = ", indent, i);

    memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);
    for (size_t j = self->elem_size; j > 0; --j) {
      fprintf(Stack::log_file, "%02hhX", elem_value[j - 1]);
    }
    fflush(Stack::log_file);

    memset(poisoned, (int) DataPoison::kDelete, self->elem_size);
    if (strcmp(elem_value, poisoned) == 0) {
      fprintf(Stack::log_file, "  okPoisoned\n");
      fflush(Stack::log_file);
    } else {
      memset(poisoned, (int) DataPoison::kFree, self->elem_size);
      if (strcmp(elem_value, poisoned) == 0) {
        fprintf(Stack::log_file, "  ERRORWrongPoison\n");
        fflush(Stack::log_file);
      } else {
        fprintf(Stack::log_file, "  ERRORNoPoison\n");
        fflush(Stack::log_file);
      }
    }
  }

  free(poisoned);
  free(elem_value);

  fprintf(Stack::log_file, "%s  }\n", indent);
  fflush(Stack::log_file);
}
