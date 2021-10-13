//
// Created by vsvood on 9/20/21.
//

#include "stack_status.h"

StackStatus operator | (StackStatus l, StackStatus r) {
  return StackStatus(unsigned(l) | unsigned(r));
}

StackStatus operator |= (StackStatus& l, StackStatus r) {
  return l = l|r;
}

StackStatus operator & (StackStatus l, StackStatus r) {
  return StackStatus(unsigned(l) & unsigned(r));
}

StackStatus operator &= (StackStatus& l, StackStatus r) {
  return l = l&r;
}
