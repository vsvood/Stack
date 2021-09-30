//
// Created by vsvood on 9/20/21.
//

#ifndef TEXTHOLDER_CUSTOM_STATUS_LIB_H
#define TEXTHOLDER_CUSTOM_STATUS_LIB_H

enum class CustomStatus {
  kOk,
  kAllocError,
  kBadStackPtr,
  kBadSelfElemSize,
  kBadSelfStackSize,
  kBadSelfStackCapacity,
  kBadSelfDataPtr,
  kBadPassedElemSize,
  kBadPassedStackCapacity,
  kBadValuePtr
};

extern const char *kCustomStatusDescription[];

#endif  // TEXTHOLDER_CUSTOM_STATUS_LIB_H
