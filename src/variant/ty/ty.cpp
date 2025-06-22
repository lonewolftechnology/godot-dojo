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
    if (ty.tag == Tag::Primitive_)
    {
        type = Type::Primitive_;
        DojoPrimitive data(ty.primitive);
        primitive = &data;
    }
    else if (ty.tag == Tag::Struct_)
    {
        type = Type::Struct_;
    }
    else if (ty.tag == Tag::Enum_)
    {
        type = Type::Array_;
    }
    else if (ty.tag == Tag::Tuple_)
    {
        type = Type::Tuple_;
    }
    else if (ty.tag == Tag::ByteArray)
    {
        type = Type::ByteArray;
    }
}

Ty::Ty(const dojo_bindings::Member& member)
{
    name = member.name;
    LOG_DEBUG("Member: ", name);
}

// Ty::Ty(const dojo_bindings::Ty* ty)
// {
//     this = {&ty};
// }

void Ty::_bind_methods()
{
}
