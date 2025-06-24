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



