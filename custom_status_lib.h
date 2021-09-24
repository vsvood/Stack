//
// Created by vsvood on 9/20/21.
//

#ifndef TEXTHOLDER_CUSTOM_STATUS_LIB_H
#define TEXTHOLDER_CUSTOM_STATUS_LIB_H

enum class CustomStatus {
  kOk,
  kWrongInputParams,
  kWrongOutputParams,
  kRuntimeError
};

extern const char *kCustomStatusDescription[];

#endif  // TEXTHOLDER_CUSTOM_STATUS_LIB_H
