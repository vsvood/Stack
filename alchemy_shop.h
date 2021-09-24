//
// Created by vsvood on 9/24/21.
//

#ifndef STACK_ALCHEMY_SHOP_H
#define STACK_ALCHEMY_SHOP_H

#include <cstdint>

enum class DataPoison : size_t{
  kFreed = 0xf0f0f0f0,
  kDeleted = 0xfdfdfdfd,
  kInvalidSize = SIZE_MAX
};

enum class PtrPoison {
  kFreed = 13,
};

#endif  // STACK_ALCHEMY_SHOP_H
