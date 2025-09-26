//
// Created by hazel on 24/06/25.
//
#include "variant/ty/enum.h"

#include "tools/logger.h"
#include "variant/ty/ty.h"

DojoEnum::DojoEnum()
{
}

DojoEnum::DojoEnum(const DOJO::Enum &enum_)
{
    value = VariantFromEnum(enum_);
}

DojoEnum::~DojoEnum()
{
}

Variant DojoEnum::VariantFromEnum(const DOJO::Enum &enum_)
{
    name = enum_.name;
    value = enum_.option;
    // TODO: Check options
    DOJO::CArrayEnumOption options = enum_.options;
    std::vector<DOJO::EnumOption> enum_vector(options.data, options.data + options.data_len);

    // string_option = enum_vector[enum_.option].name;

    Dictionary result = {};
    result["name"] = name;
    result["value"] = value;
    // result["string_value"] = string_option;
    Logger::debug_extra("DojoEnum", result);
    return result;
}
