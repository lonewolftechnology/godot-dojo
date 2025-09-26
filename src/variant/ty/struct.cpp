//
// Created by hazel on 24/06/25.
//
#include "variant/ty/struct.h"
#include "tools/logger.h"
#include "variant/ty/ty.h"

DojoStruct::DojoStruct(DOJO::Struct struct_)
{
    value = StructToVariant(struct_);
}

DojoStruct::DojoStruct(DOJO::Member member)
{
    Ref<DojoTy> t_struct = memnew(DojoTy(member));
    name = t_struct->get_name();
    value = t_struct->get_value();
}

Variant DojoStruct::StructToVariant(DOJO::Struct struct_)
{
    Dictionary result_array = {};
    name = struct_.name;
    std::vector<DOJO::Member> struct_child(struct_.children.data,
                                           struct_.children.data + struct_.children.data_len);
    for (const auto& struct_child_member : struct_child)
    {
        String child_name = struct_child_member.name;
        Ref<DojoTy> t_struct = memnew(DojoTy(struct_child_member));
        Dictionary data = {};
        data[child_name] = t_struct->get_value();
        // data["type"] = t_struct->get_name();
        Logger::debug_extra("Struct", name, child_name, t_struct->get_name());
        result_array.merge(data);
    }
    Logger::success_extra("Struct", result_array);
    return result_array;
}
