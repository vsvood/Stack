//
// Created by vsvood on 9/20/21.
//

#ifndef STACK_STACK_STATUS_H
#define STACK_STACK_STATUS_H

enum class StackStatus : unsigned {
  kOk = 0,
  kStackError = 0xff000000,
  kBadSelfPtr = 0x80000000,
  kBadSelfCapacity = 0x40000000,
  kBadSelfSize = 0x20000000,
  kBadSelfElemSize = 0x10000000,
  kBadSelfDataPtr = 0x08000000,

  kElemError = 0x00f00000,
  kBadElem = 0x00800000,

  kElemWarn = 0x000f0000,
  kElemPoisonDelete = 0x00080000,
  kElemPoisonFreed = 0x00040000,

  kParamError = 0x0000ff00,
  kBadPassedElemSize = 0x00008000,
  kBadPassedStackCapacity = 0x00004000,
  kBadPassedValuePtr = 0x00002000,

  kRuntimeError = 0x000000ff,
  kAllocError = 0x00000080,

  kUnknown = 0xffffffff
};

StackStatus operator | (StackStatus l, StackStatus r);

StackStatus operator |= (StackStatus& l, StackStatus r);

StackStatus operator & (StackStatus l, StackStatus r);

StackStatus operator &= (StackStatus& l, StackStatus r);

extern const char *kCustomStatusDescription[];

#endif  // STACK_STACK_STATUS_H
