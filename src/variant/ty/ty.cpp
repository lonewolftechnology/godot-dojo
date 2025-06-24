//
// Created by hazel on 14/06/25.
//

#include "variant/ty/ty.h"

#include "variant/ty/struct.h"
#define COLORED_TYPE(class_type, type) \
LOG_DEBUG_EXTRA(class_type, "[color=red][b]" type "[/b][/color] ")
DojoTy::DojoTy()
{
    value = Variant();
}

DojoTy::~DojoTy()
{
}

DojoTy::DojoTy(const DOJO::Ty& ty)
{
    init_form_ty(ty);
}

DojoTy::DojoTy(const DOJO::Member& member)
{
    name = member.name;
    LOG_DEBUG_EXTRA("Member", name);
    init_form_ty(*member.ty);
}

void DojoTy::init_form_ty(const DOJO::Ty& ty)
{
    if (ty.tag == Tag::Primitive_)
    {
        COLORED_TYPE("DojoTy", "primitive");
        type = Type::Primitive_;
        DojoPrimitive data = {ty.primitive};
        value = data.get_value();
        is_felt = data.is_felt;
    }
    else if (ty.tag == Tag::Struct_)
    {
        COLORED_TYPE("DojoTy", "struct");
        type = Type::Struct_;
        DojoStruct data = {ty.struct_};
        value = data.get_value();
        LOG_DEBUG(name, data.get_name());
        name = data.get_name();
    }
    else if (ty.tag == Tag::Enum_)
    {
        COLORED_TYPE("DojoTy", "enum");
        type = Type::Enum_;
    }
    else if (ty.tag == Tag::Array_)
    {
        COLORED_TYPE("DojoTy", "array");
        type = Type::Array_;
    }
    else if (ty.tag == Tag::Tuple_)
    {
        COLORED_TYPE("DojoTy", "tuple");
        type = Type::Tuple_;
    }
    else if (ty.tag == Tag::ByteArray)
    {
        COLORED_TYPE("DojoTy", "bytearray");
        type = Type::ByteArray;
    }
}


void DojoTy::_bind_methods()
{
}
