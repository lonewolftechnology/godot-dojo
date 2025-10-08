//
// Created by hazel on 26/05/25.
//
#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H
#include "dojo.h" // 1.7.5

#define DOJO dojo_bindings
#define GET_DOJO_ERROR(result) ((result).err.message)
#define GET_DOJO_OK(result) ((result).ok)
#define GET_DOJO_OK_PTR(result) ((&result).ok)

#endif // DOJO_TYPES_H
