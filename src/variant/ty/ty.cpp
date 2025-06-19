//
// Created by hazel on 14/06/25.
//

#include "variant/ty/ty.h"

#include "variant/primitive.h"

Ty::Ty()
{
    value = Variant();
}

Ty::~Ty()
{
}

Ty::Ty(const DOJO::Ty& ty)
{
    if (ty.IsPrimitive_())
    {
        type = Type::Primitive_;
        DojoPrimitive data(GET_DOJO_VALUE(ty.primitive));
        primitive = &data;
    }
    else if (ty.IsStruct_())
    {
        type = Type::Struct_;
    }
    else if (ty.IsArray_())
    {
        type = Type::Array_;
    }
    else if (ty.IsTuple_())
    {
        type = Type::Tuple_;
    }
    else if (ty.IsByteArray())
    {
        type = Type::ByteArray;
    }
}

void Ty::_bind_methods()
{
}
