//
// Created by hazel on 14/06/25.
//

#include "variant/ty/ty.h"

#include "variant/ty/array_ty.h"
#include "variant/ty/enum.h"
#include "variant/ty/struct.h"
// Esto solo para debug, no va a formar parte del logger.
#define COLORED_TYPE(class_type, type) \
LOG_DEBUG_EXTRA(class_type, "[color=red][b]" type "[/b][/color] ")

DojoTy::DojoTy()
{
    value = Variant();
}

DojoTy::~DojoTy()
{
}

DojoTy::DojoTy(const DOJO::Ty &ty)
{
    init_form_ty(ty);
}

DojoTy::DojoTy(const DOJO::Ty* ty)
{
    init_form_ty(*ty);
}

DojoTy::DojoTy(const DOJO::Member &member)
{
    name = member.name;
    LOG_DEBUG_EXTRA("Member", name);
    init_form_ty(*member.ty);
}

DojoTy::DojoTy(const dojo_bindings::CArrayTy& array_ty)
{

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
        DojoEnum data = {ty.enum_};
        value = data.get_value();
        name = data.get_name();
        Logger::debug_extra("TY ENUM", name);
    }
    else if (ty.tag == Tag::Array_)
    {
        COLORED_TYPE("DojoTy", "array");
        type = Type::Array_;
        ArrayDojoTy data = {ty.array};
        name = data.get_name();
        value = data.get_value();
    }
    else if (ty.tag == Tag::Tuple_)
    {
        COLORED_TYPE("DojoTy", "tuple");
        type = Type::Tuple_;
        ArrayDojoTy data = {ty.tuple};
        name = data.get_name();
        value = data.get_value();
    }
    else if (ty.tag == Tag::ByteArray)
    {
        COLORED_TYPE("DojoTy", "bytearray");
        type = Type::ByteArray;
        String data = ty.byte_array;
        value = data;
        Logger::debug_extra("TY BYTEARRAY", data);
    }
}


void DojoTy::_bind_methods()
{
}
