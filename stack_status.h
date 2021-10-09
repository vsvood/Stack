//
// Created by vsvood on 9/20/21.
//

#ifndef STACK_STACK_STATUS_H
#define STACK_STACK_STATUS_H

enum class StackStatus : unsigned {
  kOk                     = 0,

  kError                  = 0xffffff00,

  kSelfError              = 0xff000000,
  kBadSelfPtr             = 0x80000000,
  kBadSelfCapacity        = 0x40000000,
  kBadSelfSize            = 0x20000000,
  kBadSelfElemSize        = 0x10000000,
  kBadSelfDataPtr         = 0x08000000,

  kBadSelfSecurity        = 0x07000000,
  kBadSelfLeftCanary      = 0x04000000,
  kBadSelfRightCanary     = 0x02000000,
  kBadSelfHash            = 0x01000000,

  kElemError              = 0x00f00000,
  kBadElem                = 0x00800000,
  kBadElemPoison          = 0x00400000,
  kBadElemDel             = 0x00200000,
  kBadElemCpy             = 0x00100000,

  kParamError             = 0x000f0000,
  kBadPassedElemSize      = 0x00080000,
  kBadPassedStackCapacity = 0x00040000,
  kBadPassedValuePtr      = 0x00020000,

  kRuntimeError           = 0x0000ff00,
  kAllocError             = 0x00008000,

  kWarning                = 0x000000ff,

  kWarnSelfVoid           = 0x00000080,
  kWarnSelfCtorInfo       = 0x00000040,

  kElemWarn               = 0x00000008,
  kWarnElemPoisonDelete   = 0x00000004,
  kWarnElemPoisonFree     = 0x00000002,
  kWarnElemVerify         = 0x00000001,

  kUnknown                = 0xffffffff
};

StackStatus operator | (StackStatus l, StackStatus r);

StackStatus operator |= (StackStatus& l, StackStatus r);

StackStatus operator & (StackStatus l, StackStatus r);

StackStatus operator &= (StackStatus& l, StackStatus r);

#endif  // STACK_STACK_STATUS_H
