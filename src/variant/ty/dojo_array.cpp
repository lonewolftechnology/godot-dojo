//
// Created by hazel on 24/06/25.
//
#include "variant/ty/dojo_array.h"

#include "variant/ty/struct.h"
#include "variant/ty/ty.h"

ArrayDojo::ArrayDojo(DOJO::CArrayTy array)
{
    value = CArrayTyToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayToken array)
{
    value = CArrayTokenToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayEntity array)
{
    value = CArrayEntityToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayc_char array)
{
    value = CArrayc_charToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayClause array)
{
    value = CArrayClauseToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayStruct array)
{
    value = CArrayStructToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayMember array)
{
    value = CArrayMemberToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayOrderBy array)
{
    value = CArrayOrderByToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayEnumOption array)
{
    value = CArrayEnumOptionToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayController array)
{
    value = CArrayControllerToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayMemberValue array)
{
    value = CArrayMemberValueToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayFieldElement array)
{
    value = CArrayFieldElementToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayTokenCollection array)
{
    value = CArrayTokenCollectionToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayCOptionFieldElement array)
{
    value = CArrayCOptionFieldElementToVariant(array);
}

ArrayDojo::ArrayDojo(DOJO::CArrayCHashItemFieldElementModelMetadata array)
{
    value = CArrayCHashItemFieldElementModelMetadataToVariant(array);
}

// "Static" methods
Variant ArrayDojo::CArrayTyToVariant(DOJO::CArrayTy array_ty)
{
    Logger::debug_extra("ArrayTy", array_ty.data);
    Logger::debug_extra("ArrayTy", array_ty.data_len);
    std::vector<DOJO::Ty> array_ty_vector(array_ty.data, array_ty.data + array_ty.data_len);
    Array result = {};
    for (const auto& array_ty_element : array_ty_vector)
    {
        DojoTy dojo_ty = {array_ty_element};
        Logger::debug_extra("ArrayTy", dojo_ty.get_name());
        Logger::debug_extra("ArrayTy", dojo_ty.get_value());
        Dictionary data = {};
        data[dojo_ty.get_name()] = dojo_ty.get_value();
        result.append(data);
    }
    return result;
}


Variant ArrayDojo::CArrayTokenToVariant(DOJO::CArrayToken array)
{
    return Variant();
};

Variant ArrayDojo::CArrayEntityToVariant(DOJO::CArrayEntity array)
{
    Array result = {};
    Logger::debug_extra("ArrayEntity", array.data);
    Logger::debug_extra("ArrayEntity", array.data_len);
    std::vector<DOJO::Entity> array_entity_vector(array.data, array.data + array.data_len);
    for (const auto& entity : array_entity_vector)
    {
        Dictionary data = {};
        data["hashed_keys"] = FieldElement::get_as_string_no_ptr(entity.hashed_keys);

        ArrayDojo models = {entity.models};

        data["models"] = models.get_value();

        result.append(data);

    }
    return result;
};

Variant ArrayDojo::CArrayc_charToVariant(DOJO::CArrayc_char array)
{
    return Variant();
};

Variant ArrayDojo::CArrayClauseToVariant(DOJO::CArrayClause array)
{
    return Variant();
};

Variant ArrayDojo::CArrayStructToVariant(DOJO::CArrayStruct array)
{
    Array result = {};
    std::vector<DOJO::Struct> array_struct_vector(array.data, array.data + array.data_len);
    for (const auto& struct_ : array_struct_vector)
    {
        Dictionary data = {};
        DojoStruct dojo_struct = {struct_};
        data[dojo_struct.get_name()] = dojo_struct.get_value();
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayMemberToVariant(DOJO::CArrayMember array)
{
    return Variant();
};

Variant ArrayDojo::CArrayOrderByToVariant(DOJO::CArrayOrderBy array)
{
    return Variant();
};

Variant ArrayDojo::CArrayEnumOptionToVariant(DOJO::CArrayEnumOption array)
{
    return Variant();
};

Variant ArrayDojo::CArrayControllerToVariant(DOJO::CArrayController array)
{
    return Variant();
};

Variant ArrayDojo::CArrayMemberValueToVariant(DOJO::CArrayMemberValue array)
{
    return Variant();
};

Variant ArrayDojo::CArrayFieldElementToVariant(DOJO::CArrayFieldElement array)
{
    FieldElement felt = {array.data};
    String testing = felt.parse_cairo();
    Dictionary data = {};
    data["address"] = felt.to_string();
    data["data"] = felt.bytearray_deserialize();
    // data["data_len"] = array.data_len;
    data["data_len"] = static_cast<int64_t>(array.data_len);
    return data;
};

Variant ArrayDojo::CArrayTokenCollectionToVariant(DOJO::CArrayTokenCollection array)
{
    return Variant();
};

Variant ArrayDojo::CArrayCOptionFieldElementToVariant(DOJO::CArrayCOptionFieldElement array)
{
    return Variant();
};

Variant ArrayDojo::CArrayCHashItemFieldElementModelMetadataToVariant(
    DOJO::CArrayCHashItemFieldElementModelMetadata array)
{
    return Variant();
};
