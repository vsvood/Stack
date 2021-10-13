//
// Created by vsvood on 9/30/21.
//

#ifndef STACK_ELEM_INTERFACE_H
#define STACK_ELEM_INTERFACE_H

#include "elem_status.h"

struct ElemInterface {
    ElemStatus (*Cpy)(void* dest, const void* src);
    ElemStatus (*Dtor)(void* elem);
    ElemStatus (*Dump)(const void* elem, const char* indent);
    ElemStatus (*Verify)(const void* elem);
};

#endif //STACK_ELEM_INTERFACE_H
