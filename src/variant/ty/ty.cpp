//
// Created by hazel on 14/06/25.
//

#include "variant/ty/ty.h"

#include "variant/ty/dojo_array.h"
#include "variant/ty/enum.h"
#include "variant/ty/struct.h"

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

DojoTy::DojoTy(const DOJO::Ty* ty)
{
    init_form_ty(*ty);
}

DojoTy::DojoTy(const DOJO::Member& member)
{
    name = member.name;
    Logger::custom("Member", name);
    if (member.ty != nullptr)
    {
        init_form_ty(*member.ty);
    }
    else
    {
        Logger::custom("DojoTy", "Member ty is null");
    }
}

DojoTy::DojoTy(const dojo_bindings::CArrayTy& array_ty)
{
}

void DojoTy::init_form_ty(const DOJO::Ty& ty)
{
    if (ty.tag == Tag::Primitive_)
    {
        type = Type::Primitive_;
        DojoPrimitive data = {ty.primitive};
        value = data.get_value();
        is_felt = data.is_felt;
    }
    else if (ty.tag == Tag::Struct_)
    {
        type = Type::Struct_;
        DojoStruct data = {ty.struct_};
        value = data.get_value();
        name = data.get_name();
    }
    else if (ty.tag == Tag::Enum_)
    {
        type = Type::Enum_;
        DojoEnum data = {ty.enum_};
        value = data.get_value();
        name = data.get_name();
    }
    else if (ty.tag == Tag::Array_)
    {
        type = Type::Array_;
        ArrayDojo data = {ty.array};
        name = data.get_name();
        value = data.get_value();
    }
    else if (ty.tag == Tag::Tuple_)
    {
        type = Type::Tuple_;
        ArrayDojo data = {ty.tuple};
        name = data.get_name();
        value = data.get_value();
    }
    else if (ty.tag == Tag::ByteArray)
    {
        type = Type::ByteArray;
        String data = ty.byte_array;
        value = data;
    }
}


void DojoTy::_bind_methods()
{
}
