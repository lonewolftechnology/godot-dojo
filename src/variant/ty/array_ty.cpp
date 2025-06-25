//
// Created by hazel on 24/06/25.
//
#include "variant/ty/array_ty.h"

#include "variant/ty/ty.h"

ArrayDojoTy::ArrayDojoTy()
{

}

ArrayDojoTy::~ArrayDojoTy()
{

}

ArrayDojoTy::ArrayDojoTy(DOJO::CArrayTy array_ty)
{
    value = ArrayDojoTyToVariant(array_ty);
}

ArrayDojoTy::ArrayDojoTy(dojo_bindings::CArrayFieldElement array_felt)
{
    value = ArrayDojoFieldElementToVariant(array_felt);
}

Variant ArrayDojoTy::ArrayDojoTyToVariant(DOJO::CArrayTy array_ty)
{
    Array result = {};

    std::vector<DOJO::Ty> array_ty_vector(array_ty.data, array_ty.data + array_ty.data_len);

    for (const auto& array_ty_element : array_ty_vector)
    {
        DojoTy dojo_ty = {array_ty_element};
        Logger::debug_extra("ArrayDojoTy", dojo_ty.get_name());
        Logger::debug_extra("ArrayDojoTy", dojo_ty.get_value());
        Dictionary data = {};
        data[dojo_ty.get_name()] = dojo_ty.get_value();
        result.append(data);
    }

    return result;
}

Variant ArrayDojoTy::ArrayDojoFieldElementToVariant(DOJO::CArrayFieldElement array_felt)
{
    FieldElement felt = {array_felt.data};
    String testing = felt.parse_cairo();
    Dictionary data = {};
    data["address"] = felt.to_string();
    data["data"] = felt.parse_cairo();
    // data["data_len"] = array_felt.data_len;
    data["data_len"] = static_cast<int64_t>(array_felt.data_len);
    return data;
}

