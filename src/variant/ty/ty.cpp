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
    if (member.ty != nullptr)
    {
        init_form_ty(*member.ty);
    }
    else
    {
        Logger::custom("DojoTy", "Member ty is null");
    }
    Logger::custom("MemberName", name, member.name);
    name = member.name;

}

DojoTy::DojoTy(const dojo_bindings::CArrayTy& array_ty)
{
}

void DojoTy::init_form_ty(const DOJO::Ty& ty)
{
    Logger::success_extra("TY", static_cast<int>(ty.tag));
    if (ty.tag == Tag::Primitive_)
    {
        type = Type::Primitive_;
        Ref<DojoPrimitive> data = memnew(DojoPrimitive(ty.primitive));
        value = data->get_value();
        is_felt = data->is_felt;
    }
    else if (ty.tag == Tag::Struct_)
    {
        type = Type::Struct_;
        Ref<DojoStruct> data = memnew(DojoStruct(ty.struct_));
        value = data->get_value();
        name = data->get_name();
    }
    else if (ty.tag == Tag::Enum_)
    {
        type = Type::Enum_;
        Ref<DojoEnum> data = memnew(DojoEnum(ty.enum_));
        value = data->get_value();
        name = data->get_name();
    }
    else if (ty.tag == Tag::Array_)
    {
        type = Type::Array_;
        Ref<DojoArray> data = memnew(DojoArray(ty.array));
        name = data->get_name();
        value = data->get_value();
    }
    else if (ty.tag == Tag::Tuple_)
    {
        type = Type::Tuple_;
        Ref<DojoArray> data = memnew(DojoArray(ty.tuple));
        name = data->get_name();
        value = data->get_value();
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
