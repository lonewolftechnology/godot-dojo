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
    LOG_CUSTOM("Struct", member.name);
    DojoTy t_struct = DojoTy(member);
    name = t_struct.get_name();
    LOG_CUSTOM("DOJOTY", name, " ->>><<<-");
    value = t_struct.get_value();
}

Variant DojoStruct::StructToVariant(DOJO::Struct struct_)
{
    Array result_array = {};
    name = struct_.name;
    LOG_CUSTOM("DojoStruct", name);
    std::vector<DOJO::Member> struct_child(struct_.children.data,
                                           struct_.children.data + struct_.children.data_len);
    if (name.to_lower().contains("vec2"))
    {
        name = "Vector2";
        Vector2 vec = {0, 0};
        for (const auto& struct_child_member : struct_child)
        {
            String member_name = struct_child_member.name;
            DojoTy v_struct = DojoTy(struct_child_member);

            if (member_name.to_lower().contains("x"))
            {
                vec.x = v_struct.get_value();
            }
            else
            {
                vec.y = v_struct.get_value();
            }
        }
        return vec;
    }
    LOG_CUSTOM("Struct", name);
    for (const auto& struct_child_member : struct_child)
    {
        name = struct_child_member.name;
        LOG_CUSTOM("Nested", name);
        DojoTy t_struct = DojoTy(struct_child_member);
        Dictionary data = {};
        data[t_struct.get_name()] = t_struct.get_value();
        result_array.append(data);
    }
    LOG_DEBUG_EXTRA("FINAL STRUCT", result_array);
    return result_array;
}
