//
// Created by vsvood on 9/23/21.
//

#include "stack.h"

#include <cstring>
#include <cmath>
#include <cstdio>

#include "alchemy_shop.h"

StackStatus CheckCtorParams(Stack *self, size_t elem_size, size_t capacity);

void SetExtraInfo(Stack *self, ExtraInfo extra_info);

StackStatus Stack::Ctor(Stack *self, size_t elem_size, size_t capacity, ElemInterface interface, ExtraInfo extra_info) {
  StackStatus status = CheckCtorParams(self, elem_size, capacity);
  if ((status & StackStatus::kError) != StackStatus::kOk) {
    return status;
  }
  SetExtraInfo(self, extra_info);

  self->elem_interface = interface;
  self->data = nullptr;
  self->elem_size = elem_size;
  self->capacity = capacity;
  self->size = 0;

  status = SmartRealloc(self, self->size, capacity);
  if (status != StackStatus::kOk) {
    return status;
  }

  if (self->data != nullptr) {
    memset(self->data, (int)DataPoison::kDeleted, self->elem_size * self->capacity);
  }

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = 0;
  for (char* elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
    self->hash ^= ((uint64_t)*elem_p)<<((uint64_t)elem_p%56);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  return StackStatus::kOk;
}

StackStatus CheckCtorParams(Stack *self, size_t elem_size, size_t capacity) {
  StackStatus status = Stack::Verify(self);
  if (capacity >= (size_t) DataPoison::kInvalidSize) {
    status |= StackStatus::kBadPassedStackCapacity;
  }
  if (elem_size == 0 || elem_size >= (size_t)DataPoison::kInvalidSize) {
    status |= StackStatus::kBadPassedElemSize;
  }
  return status;
}

void SetExtraInfo(Stack *self, ExtraInfo extra_info) {
  if (extra_info.var_name != nullptr) {
    self->name = (char*) calloc(strlen(extra_info.var_name) + 1, 1);
    if (self->name)
      strcpy(self->name, extra_info.var_name);

    self->type = (char*) calloc(strlen(extra_info.type_name) + 1, 1);
    if (self->type)
      strcpy(self->type, extra_info.type_name);

    self->ctor_at_func = (char*) calloc(strlen(extra_info.func_name) + 1, 1);
    if (self->ctor_at_func)
      strcpy(self->ctor_at_func, extra_info.func_name);

    self->ctor_in_file = (char*) calloc(strlen(extra_info.file_name) + 1, 1);
    if (self->ctor_in_file)
      strcpy(self->ctor_in_file, extra_info.file_name);

    self->ctor_on_line = extra_info.line;
  }
}

void FreeExtraInfo(Stack* self, StackStatus status);

StackStatus Stack::Dtor(Stack* self) {
  StackStatus status = Verify(self);
  if ((status & StackStatus::kSelfError) != StackStatus::kOk) {
    return status;
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

  FreeExtraInfo(self, status);

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = 0;
  for (char* elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
    self->hash ^= ((uint64_t)*elem_p)<<((uint64_t)elem_p%56);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  return StackStatus::kOk;
}

void FreeExtraInfo(Stack* self, StackStatus status) {
  if ((status & StackStatus::kWarnSelfCtorName) == StackStatus::kOk) {
    free(self->name);
    self->name = (char *) PtrPoison::kFreed;
  }
  if ((status & StackStatus::kWarnSelfCtorType) == StackStatus::kOk) {
    free(self->type);
    self->type = (char *) PtrPoison::kFreed;
  }
  if ((status & StackStatus::kWarnSelfCtorFunc) == StackStatus::kOk) {
    free(self->ctor_at_func);
    self->ctor_at_func = (char *) PtrPoison::kFreed;
  }
  if ((status & StackStatus::kWarnSelfCtorFile) == StackStatus::kOk) {
    free(self->ctor_in_file);
    self->ctor_in_file = (char *) PtrPoison::kFreed;
  }
  if ((status & StackStatus::kWarnSelfCtorLine) == StackStatus::kOk) {
    self->ctor_on_line = (int) DataPoison::kFreed;
  }
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

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = 0;
  for (char* elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
    self->hash ^= ((uint64_t)*elem_p)<<((uint64_t)elem_p%56);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

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

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = 0;
  for (char* elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
    self->hash ^= ((uint64_t)*elem_p)<<((uint64_t)elem_p%56);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

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

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  self->hash = 0;
  for (char* elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
    self->hash ^= ((uint64_t)*elem_p)<<((uint64_t)elem_p%56);
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  return StackStatus::kOk;
}

StackStatus Stack::Verify(const Stack *self) {
  if (self == nullptr || self == (Stack*)PtrPoison::kFreed) {
    return StackStatus::kBadSelfPtr;
  }
  if (self->is_verifying)
    return StackStatus::kOk;
  self->is_verifying = true;
  StackStatus status = StackStatus::kOk;

  if (self->name == nullptr || self->name == (char*)PtrPoison::kFreed)
    status |= StackStatus::kWarnSelfCtorName;
  if (self->type == nullptr || self->type == (char*)PtrPoison::kFreed)
    status |= StackStatus::kWarnSelfCtorType;
  if (self->ctor_at_func == nullptr ||
      self->ctor_at_func == (char*)PtrPoison::kFreed)
    status |= StackStatus::kWarnSelfCtorFunc;
  if (self->ctor_in_file == nullptr ||
      self->ctor_in_file == (char*)PtrPoison::kFreed)
    status |= StackStatus::kWarnSelfCtorFile;
  if (self->ctor_on_line == 0)
    status |= StackStatus::kWarnSelfCtorLine;

  if (self->capacity == (size_t)DataPoison::kInvalidSize)
    status |= StackStatus::kBadSelfCapacity;
  if (self->capacity < self->size ||
      self->size == (size_t)DataPoison::kInvalidSize)
    status |= StackStatus::kBadSelfSize;
  if ((self->capacity != 0 && self->elem_size == 0) ||
      self->elem_size == (size_t)DataPoison::kInvalidSize)
    status |= StackStatus::kBadSelfElemSize;
  if (self->capacity != 0 &&
      (self->data == nullptr || self->data == (char*)PtrPoison::kFreed))
    status |= StackStatus::kBadSelfDataPtr;

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
  if (self->left_canary != (size_t)DataPoison::kCanary)
    status |= StackStatus::kBadSelfLeftCanary;
  if (self->right_canary != (size_t)DataPoison::kCanary)
    status |= StackStatus::kBadSelfRightCanary;
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
  if (self->data) {
    uint64_t cur_hash = 0;
    for (const char *elem_p = &(self->left_hash_reference); elem_p < &(self->right_hash_reference); ++elem_p) {
      cur_hash ^= ((uint64_t) *elem_p) << ((uint64_t) elem_p % 56);
    }
    if (cur_hash != self->hash) {
      status |= StackStatus::kBadSelfHash;
    }
  }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

  if ((status & StackStatus::kSelfError) != StackStatus::kOk) {
    self->is_verifying = false;
    return status;
  }

  if (self->elem_interface.Verify != nullptr)
    for (size_t i = 0; i < self->size; ++i) {
      ElemStatus elem_status = self->elem_interface.Verify(&self->data[i*self->elem_size]);
      if (elem_status == ElemStatus::kCorrupted)
        status |= StackStatus::kBadElem;
      else if (elem_status == ElemStatus::kWarning)
        status |= StackStatus::kWarnElemVerify;
    }
  self->is_verifying = false;
  return status;
}

StackStatus Stack::Dump(const Stack *self, ExtraInfo extra_info, const char *indent) {
  StackStatus status = Stack::Verify(self);

  if ((status & StackStatus::kBadSelfPtr) == StackStatus::kOk) {
    if (self->is_dumping) {
      fprintf(self->log_file, "%s cycles to -stack<%s> [%p] ok\n", indent,
              ((status & StackStatus::kWarnSelfCtorType) == StackStatus::kOk)
              ? self->type : "", self);
      fflush(self->log_file);
      return StackStatus::kOk;
    }

    self->is_dumping = true;
    fprintf(self->log_file, "%s-stack<%s> [%p] %s", indent,
            ((status & StackStatus::kWarnSelfCtorType) == StackStatus::kOk)
            ? self->type : "", self,
            ((status & StackStatus::kError) == StackStatus::kOk)
            ? "ok" : "ERROR");

    if (extra_info.var_name != nullptr) {
      fprintf(self->log_file, " \"%s\" at %s at %s(%d)",
              extra_info.var_name, extra_info.func_name,
              extra_info.file_name, extra_info.line);
    }
    fprintf(self->log_file, "\n");
    fflush(self->log_file);

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY
    if ((status & StackStatus::kBadSelfLeftCanary) != StackStatus::kOk) {
      fprintf(self->log_file, "%s  ERROR self left canary damaged! stack status (%d)\n", indent, (int)status);
      fflush(self->log_file);
      return status;
    }
    if ((status & StackStatus::kBadSelfRightCanary) != StackStatus::kOk) {
      fprintf(self->log_file, "%s  ERROR self right canary damaged! stack status (%d)\n", indent, (int)status);
      fflush(self->log_file);
      return status;
    }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_CANARY

#if STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH
    if ((status & StackStatus::kBadSelfHash) != StackStatus::kOk) {
      fprintf(self->log_file, "%s  ERROR self hash differs! stack status (%d)\n", indent, (int)status);
      fflush(self->log_file);
      return status;
    }
#endif  // STACK_SECURE_LEVEL & STACK_SECURE_SELF_HASH

      if ((status & StackStatus::kWarnSelfCtorData) == StackStatus::kOk) {
        fprintf(self->log_file, "%s  Ctor as \"%s\" at %s in %s(%d)\n",
                indent, self->name, self->ctor_at_func,
                self->ctor_in_file, self->ctor_on_line);
        fflush(self->log_file);
      }
    fprintf(self->log_file, "%s  capacity  - %zu %s", indent, self->capacity,
            ((status & StackStatus::kBadSelfCapacity) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(self->log_file);
    fprintf(self->log_file, "%s  size      - %zu %s", indent, self->size,
            ((status & StackStatus::kBadSelfSize) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(self->log_file);
    fprintf(self->log_file, "%s  elem_size - %zu %s", indent, self->elem_size,
            ((status & StackStatus::kBadSelfElemSize) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(self->log_file);
    fprintf(self->log_file, "%s  data      - [%p] %s", indent, self->data,
            ((status & StackStatus::kBadSelfDataPtr) != StackStatus::kOk) ? "ERROR\n" : "ok\n");
    fflush(self->log_file);

    if (self->size != 0) {
      if ((status & StackStatus::kSelfError) == StackStatus::kOk) {
        fprintf(self->log_file, "%s  {\n", indent);

        char *poisoned = (char *) calloc(1, self->elem_size + 1);
        poisoned[self->elem_size] = '\0';
        char *elem_value = (char *) calloc(1, self->elem_size + 1);
        poisoned[self->elem_size] = '\0';

        char *new_indent = (char *) calloc(1, strlen(indent) + 7);
        memccpy(new_indent, indent, 1, strlen(indent));
        new_indent = strcat(new_indent, "      ");

        for (size_t i = 0; i < self->size; ++i) {
          fprintf(self->log_file, "%s   *[%zu] = ", indent, i);

          memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);
          for (size_t j = self->elem_size; j > 0; --j) {
            fprintf(self->log_file, "%02hhX", elem_value[j - 1]);
          }
          fflush(self->log_file);

          memset(poisoned, (int) DataPoison::kDeleted, self->elem_size);
          if (strcmp(elem_value, poisoned) == 0) {
            fprintf(self->log_file, "  WARNDelPoison");
          }
          fflush(self->log_file);

          memset(poisoned, (int) DataPoison::kFreed, self->elem_size);
          if (strcmp(elem_value, poisoned) == 0) {
            fprintf(self->log_file, "  WARNFreePoison");
          }
          fflush(self->log_file);

          if (self->elem_interface.Verify != nullptr) {
            ElemStatus elem_status = self->elem_interface.Verify(&self->data[i]);
            if (elem_status == ElemStatus::kCorrupted) {
              fprintf(self->log_file, "  ERRORCorrupted\n");
              fflush(self->log_file);
            } else {
              fprintf(self->log_file, "  ok\n");
              fflush(self->log_file);
            }
            if (self->elem_interface.Dump != nullptr) {
              self->elem_interface.Dump(&self->data[i], new_indent);
            }
            fflush(self->log_file);
          } else {
            fprintf(self->log_file, "\n");
          }
          fflush(self->log_file);
        }
        free(new_indent);
        for (size_t i = self->size; i < self->capacity; ++i) {
          fprintf(self->log_file, "%s    [%zu] = ", indent, i);

          memccpy(elem_value, &self->data[i * self->elem_size], 1, self->elem_size);
          for (size_t j = self->elem_size; j > 0; --j) {
            fprintf(self->log_file, "%02hhX", elem_value[j - 1]);
          }
          fflush(self->log_file);

          memset(poisoned, (int) DataPoison::kDeleted, self->elem_size);
          if (strcmp(elem_value, poisoned) == 0) {
            fprintf(self->log_file, "  okPoisoned\n");
            fflush(self->log_file);
          } else {
            memset(poisoned, (int) DataPoison::kFreed, self->elem_size);
            if (strcmp(elem_value, poisoned) == 0) {
              fprintf(self->log_file, "  ERRORWrongPoison\n");
              fflush(self->log_file);
            } else {
              fprintf(self->log_file, "  ERRORNoPoison\n");
              fflush(self->log_file);
            }
          }
        }

        free(poisoned);
        free(elem_value);

        fprintf(self->log_file, "%s  }\n", indent);
        fflush(self->log_file);
      }
    }
    fprintf(self->log_file, "\n");
    fflush(self->log_file);

    self->is_dumping = false;
  }
  return status;
}
