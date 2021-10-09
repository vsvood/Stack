//
// Created by vsvood on 9/24/21.
//

#ifndef STACK_ALCHEMY_SHOP_H
#define STACK_ALCHEMY_SHOP_H

#include <cstdint>

enum class DataPoison : size_t{
  kFree = 0xf0,
  kDelete = 0xfd,
  kInvalidSize = SIZE_MAX,
  kCanary = 0xDeadDeadDeadDead
};

enum class PtrPoison {
  kFreed = 13,
};

#endif  // STACK_ALCHEMY_SHOP_H
