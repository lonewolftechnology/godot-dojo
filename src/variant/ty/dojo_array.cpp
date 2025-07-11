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
    value = PageControllerToVariant(array);
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
    value = CArrayModelToVariant(array);
}

// "Static" methods
Variant ArrayDojo::CArrayTyToVariant(DOJO::CArrayTy array_ty)
{
    Logger::debug_extra("ArrayTy", array_ty.data);
    Logger::debug_extra("ArrayTy", array_ty.data_len);
    std::vector array_ty_vector(array_ty.data, array_ty.data + array_ty.data_len);
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
    Logger::warning("CArrayToken not implemented");
    return Array();
};

Variant ArrayDojo::CArrayEntityToVariant(DOJO::CArrayEntity array)
{
    Array result = {};
    Logger::debug_extra("ArrayEntity", "entity amount: ", array.data_len);
    std::vector array_entity_vector(array.data, array.data + array.data_len);
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
    String result;
    const char** c_char = array.data;

    for (int i = 0; i < array.data_len; i++)
    {
        result += *c_char;
        c_char++;
    }
    Logger::debug_extra("Arrayc_char", result);
    Logger::debug_extra("Arrayc_char", array.data_len);
    return result;
};

Variant ArrayDojo::CArrayClauseToVariant(DOJO::CArrayClause array)
{
    Array result;
    std::vector array_clause_vector(array.data, array.data + array.data_len);
    for (const auto& clause : array_clause_vector)
    {
        Dictionary data = {};
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayStructToVariant(DOJO::CArrayStruct array)
{
    Array result = {};
    Logger::debug_extra("ArrayStruct", "struct amount: ", array.data_len);
    std::vector array_struct_vector(array.data, array.data + array.data_len);
    for (const auto& struct_ : array_struct_vector)
    {
        Logger::debug_extra("ArrayStruct", "struct: ", struct_.name);
        Dictionary data = {};
        DojoStruct dojo_struct = {struct_};
        Logger::debug_extra("ArrayStruct", dojo_struct.get_name(), dojo_struct.get_value());
        data[dojo_struct.get_name()] = dojo_struct.get_value();
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayMemberToVariant(DOJO::CArrayMember array)
{
    Array result;
    std::vector array_member_vector(array.data, array.data + array.data_len);
    Logger::custom("CArrayMember", "init");

    for (const auto& member : array_member_vector)
    {
        Dictionary data = {};
        DojoTy dojo_ty = {member.ty};
        String data_name = dojo_ty.get_name();
        if (data_name.is_empty())
        {
            data_name = member.name;
        }
        Logger::custom("CArrayMember", data_name);
        data[data_name] = dojo_ty.get_value();
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayOrderByToVariant(DOJO::CArrayOrderBy array)
{
    Array result = {};

    std::vector<DOJO::OrderBy> array_order_by_vector(array.data, array.data + array.data_len);
    for (const auto& orderby : array_order_by_vector)
    {
        Dictionary data = {};
        data["model"] = orderby.model;
        data["member"] = orderby.member;
        data["direction"] = static_cast<uint8_t>(orderby.direction);
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayEnumOptionToVariant(DOJO::CArrayEnumOption array)
{
    Array result = {};

    std::vector<DOJO::EnumOption> array_enum_option_vector(array.data, array.data + array.data_len);

    for (const auto& enum_option : array_enum_option_vector)
    {
        Dictionary data = {};
        data["name"] = enum_option.name;
        data["data"] = DojoTy(enum_option.ty).get_value();
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::PageControllerToVariant(const DOJO::CArrayController& array)
{
    TypedArray<Dictionary> result = {};

    std::vector<DOJO::Controller> array_controller_vector(array.data, array.data + array.data_len);
    for (const auto& controller : array_controller_vector)
    {
        Dictionary data = {};
        DOJO::FieldElement _felt = controller.address;
        FieldElement controller_addr{&_felt};
        data["address"] = controller_addr.to_string();
        data["felt_data"] = controller_addr.to_string();
        data["username"] = controller.username;
        data["deployed_at_timestamp"] = controller.deployed_at_timestamp;
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayMemberValueToVariant(DOJO::CArrayMemberValue array)
{
    Array result = {};
    std::vector array_member_value_vector(array.data, array.data + array.data_len);
    for (const auto& member_value : array_member_value_vector)
    {
        Variant data = {};
        if (member_value.tag == DOJO::List)
        {   //Recursive
            ArrayDojo array_dojo = {member_value.list};
            data = array_dojo.get_value();
        }
        if (member_value.tag == DOJO::PrimitiveValue)
        {
            DojoPrimitive dojo_ty = {member_value.primitive_value};
            data = dojo_ty.get_value();
        }
        if (member_value.tag == DOJO::String)
        {
            data = member_value.string;
        }
        result.append(data);
    }
    return result;
};

Variant ArrayDojo::CArrayFieldElementToVariant(DOJO::CArrayFieldElement array)
{
    FieldElement felt = {array.data};
    Logger::debug_extra("FieldElement", felt.to_string());
    Logger::debug_extra("FieldElement", felt.parse_cairo());
    Logger::debug_extra("FieldElement", array.data_len);
    Dictionary data = {};
    data["address"] = felt.to_string();
    data["data"] = felt.bytearray_deserialize(32);
    // data["data_len"] = array.data_len;
    data["data_len"] = static_cast<int64_t>(array.data_len);

    return data;
};

Variant ArrayDojo::CArrayTokenCollectionToVariant(DOJO::CArrayTokenCollection array)
{
    Logger::warning("CArrayTokenCollection not implemented");
    return Array();
};

Variant ArrayDojo::CArrayCOptionFieldElementToVariant(DOJO::CArrayCOptionFieldElement array)
{
    Array result = {};
    std::vector<DOJO::COptionFieldElement> array_coption_field_element_vector(array.data, array.data + array.data_len);
    for (const auto& coption_field_element : array_coption_field_element_vector)
    {
        if (coption_field_element.tag == DOJO::SomeFieldElement)
        {
            Variant data = FieldElement(coption_field_element.some).to_string();
            result.append(data);
        }
    }
    return result;
};

Variant ArrayDojo::CArrayModelToVariant(
    DOJO::CArrayCHashItemFieldElementModelMetadata array)
{
    std::vector<DOJO::CHashItemFieldElementModelMetadata> models_vec(
    array.data, array.data + array.data_len
);
    Array result = {};
    for (const auto& model_item : models_vec)
    {
        Dictionary model_dict = {};
        DOJO::ModelMetadata model = model_item.value;

        Logger::debug_extra("METADATA", model.namespace_);
        Logger::debug_extra("METADATA", model.name);

        DojoTy metadata_ty = {model.schema};
        model_dict["schema"] = metadata_ty.get_value();

        model_dict["namespace"] = model.namespace_;
        model_dict["name"] = model.name;
        model_dict["packed_size"] = Variant(model.packed_size);
        model_dict["unpacked_size"] = Variant(model.unpacked_size);
        model_dict["class_hash"] = FieldElement::get_as_string_no_ptr(model.class_hash);
        model_dict["contract_address"] = FieldElement::get_as_string_no_ptr(model.contract_address);
        ArrayDojo layout = {model.layout};
        model_dict["layout"] = layout.get_value();
        Logger::empty_line();
        result.append(model_dict);
    }
    return result;
};
