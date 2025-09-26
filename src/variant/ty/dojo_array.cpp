#include "variant/ty/dojo_array.h"
#include "variant/ty/struct.h"
#include "variant/ty/ty.h"
#include "ref_counted/options/option_clause.h"
#include "ref_counted/options/option_u256.h"

DojoArray::DojoArray(DOJO::CArrayTy array)
{
    value = CArrayTyToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayToken array)
{
    value = CArrayTokenToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayEntity array)
{
    value = CArrayEntityToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayc_char array)
{
    value = CArrayc_charToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayClause array)
{
    value = CArrayClauseToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayStruct array)
{
    value = CArrayStructToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayMember array)
{
    value = CArrayMemberToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayOrderBy array)
{
    value = CArrayOrderByToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayEnumOption array)
{
    value = CArrayEnumOptionToVariant(array);
}

DojoArray::DojoArray(DOJO::PageController array)
{
    value = PageControllerToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayMemberValue array)
{
    value = CArrayMemberValueToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayFieldElement array)
{
    value = CArrayFieldElementToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayTokenContract array)
{
    value = CArrayTokenContractToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayCOptionFieldElement array)
{
    value = CArrayCOptionFieldElementToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayModel array)
{
    value = CArrayModelToVariant(array);
}

Variant DojoArray::CArrayTyToVariant(DOJO::CArrayTy array_ty)
{
    std::vector<DOJO::Ty> array_ty_vector(array_ty.data, array_ty.data + array_ty.data_len);
    Array result;

    for (const auto& array_ty_element : array_ty_vector)
    {
        Ref<DojoTy> dojo_ty = memnew(DojoTy(array_ty_element));
        Dictionary data;
        data[dojo_ty->get_name()] = dojo_ty->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayTokenToVariant(DOJO::CArrayToken array)
{
    Array result = {};
    std::vector<DOJO::Token> array_token_vector(array.data, array.data + array.data_len);
    for (const auto& token : array_token_vector)
    {
        Dictionary data;
        data["contract_address"] = FieldElement::get_as_string_no_ptr(token.contract_address);
        Ref<OptionU256> token_id = memnew(OptionU256(token.token_id));
        data["token_id"] = token_id->get_value();
        data["name"] = token.name;
        data["symbol"] = token.symbol;
        data["decimals"] = token.decimals;
        data["metadata"] = token.metadata;
        Ref<OptionU256> total_supply = memnew(OptionU256(token.total_supply));
        data["total_supply"] = total_supply->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayEntityToVariant(DOJO::CArrayEntity array)
{
    Array result = {};
    Logger::debug_extra("CArrayEntityToVariant", "array.data_len: ", array.data_len);
    std::vector array_entity_vector(array.data, array.data + array.data_len);

    for (const auto& entity : array_entity_vector)
    {
        Dictionary data;

        data["hashed_keys"] = FieldElement::get_as_string_no_ptr(entity.hashed_keys);
        data["models"] = CArrayStructToVariant(entity.models);

        result.append(data);
    }
    Logger::success_extra("CArrayEntityToVariant", result);
    return result;
}

Variant DojoArray::CArrayc_charToVariant(DOJO::CArrayc_char array)
{
    Array result_array;
    std::vector<const char*> strings(array.data, array.data + array.data_len);

    for (const auto* c_str : strings)
    {
        result_array.append(String(c_str));
    }
    return result_array;
}

Variant DojoArray::CArrayClauseToVariant(DOJO::CArrayClause array)
{
    Array result;
    std::vector array_clause_vector(array.data, array.data + array.data_len);

    for (const auto& clause : array_clause_vector)
    {
        Dictionary data;
        result.append(OptionClause::from_native(clause));
    }
    return result;
}

Variant DojoArray::CArrayStructToVariant(DOJO::CArrayStruct array)
{
    Array result;
    std::vector<DOJO::Struct> array_struct_vector(array.data, array.data + array.data_len);

    for (const auto& struct_ : array_struct_vector)
    {
        Dictionary data;
        Ref<DojoStruct> dojo_struct = memnew(DojoStruct(struct_));

        data[dojo_struct->get_name()] = dojo_struct->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayMemberToVariant(DOJO::CArrayMember array)
{
    Array result;
    std::vector<DOJO::Member> array_member_vector(array.data, array.data + array.data_len);

    for (const auto& member : array_member_vector)
    {
        Dictionary data;
        Ref<DojoTy> dojo_ty = memnew(DojoTy(member.ty));
        String data_name = dojo_ty->get_name();
        if (data_name.is_empty())
        {
            data_name = member.name;
        }
        data[data_name] = dojo_ty->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayOrderByToVariant(DOJO::CArrayOrderBy array)
{
    Array result;
    std::vector<DOJO::OrderBy> array_order_by_vector(array.data, array.data + array.data_len);

    for (const auto& orderby : array_order_by_vector)
    {
        Dictionary data;
        data["field"] = orderby.field;
        data["direction"] = static_cast<uint8_t>(orderby.direction);
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayEnumOptionToVariant(DOJO::CArrayEnumOption array)
{
    Array result;
    std::vector<DOJO::EnumOption> array_enum_option_vector(array.data, array.data + array.data_len);

    for (const auto& enum_option : array_enum_option_vector)
    {
        Dictionary data;
        data["name"] = enum_option.name;
        data["data"] = memnew(DojoTy(enum_option.ty))->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::PageControllerToVariant(const DOJO::PageController& page)
{
    TypedArray<Dictionary> result;
    DOJO::CArrayController array = page.items;
    std::vector<DOJO::Controller> array_controller_vector(array.data, array.data + array.data_len);

    for (const auto& controller : array_controller_vector)
    {
        Dictionary data;
        Ref<FieldElement> controller_addr = memnew(FieldElement(controller.address));
        data["address"] = controller_addr->to_string();
        data["felt_data"] = controller_addr->to_string();
        data["username"] = controller.username;
        data["deployed_at_timestamp"] = controller.deployed_at_timestamp;
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayMemberValueToVariant(DOJO::CArrayMemberValue array)
{
    Array result;
    std::vector<DOJO::MemberValue> array_member_value_vector(array.data, array.data + array.data_len);

    for (const auto& member_value : array_member_value_vector)
    {
        Variant data;
        if (member_value.tag == DOJO::List)
        {
            Ref<DojoArray> array_dojo = memnew(DojoArray(member_value.list));
            data = array_dojo->get_value();
        }
        if (member_value.tag == DOJO::PrimitiveValue)
        {
            Ref<DojoPrimitive> dojo_ty = memnew(DojoPrimitive(member_value.primitive_value));
            data = dojo_ty->get_value();
        }
        if (member_value.tag == DOJO::String)
        {
            data = member_value.string;
        }
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayFieldElementToVariant(DOJO::CArrayFieldElement array)
{
    Array result_array;
    std::vector<DOJO::FieldElement> elements(array.data, array.data + array.data_len);

    for (const auto& element : elements)
    {
        Ref<FieldElement> felt = memnew(FieldElement(element));
        result_array.append(felt->to_string());
    }
    return result_array;
}

Variant DojoArray::CArrayTokenContractToVariant(DOJO::CArrayTokenContract array)
{
    Array result;
    std::vector<DOJO::TokenContract> elements(array.data, array.data + array.data_len);

    for (const auto& element : elements)
    {
        Dictionary element_dict = {};

        Ref<FieldElement> contract_address = memnew(FieldElement(element.contract_address));
        element_dict["contract_address"] = contract_address->to_string();
        element_dict["name"] = element.name;
        element_dict["symbol"] = element.symbol;
        element_dict["decimals"] = element.decimals;
        element_dict["metadata"] = element.metadata;
        element_dict["total_supply"] = memnew(OptionU256(element.total_supply));

        result.append(element_dict);
    }
    return result;
}

Variant DojoArray::CArrayCOptionFieldElementToVariant(DOJO::CArrayCOptionFieldElement array)
{
    Array result;
    std::vector<DOJO::COptionFieldElement> options(array.data, array.data + array.data_len);

    for (const auto& option : options)
    {
        if (option.tag == DOJO::SomeFieldElement)
        {
            result.append(FieldElement(option.some).to_string());
        }
    }
    return result;
}

Variant DojoArray::CArrayModelToVariant(DOJO::CArrayModel array)
{
    std::vector<DOJO::Model> models_vec(array.data, array.data + array.data_len);
    Array result;

    for (const auto& model : models_vec)
    {
        Dictionary model_dict;

        Ref<FieldElement> selector_felt = memnew(FieldElement(model.selector));
        Ref<DojoTy> metadata_ty = memnew(DojoTy(model.schema));
        model_dict["schema"] = metadata_ty->get_value();
        model_dict["namespace"] = model.namespace_;
        model_dict["name"] = model.name;
        model_dict["packed_size"] = Variant(model.packed_size);
        model_dict["unpacked_size"] = Variant(model.unpacked_size);
        model_dict["selector"] = FieldElement::get_as_string_no_ptr(model.selector);
        model_dict["selector_txt1"] = selector_felt->bytearray_deserialize(32);
        model_dict["selector_txt2"] = selector_felt->parse_cairo();
        model_dict["class_hash"] = FieldElement::get_as_string_no_ptr(model.class_hash);
        model_dict["contract_address"] = FieldElement::get_as_string_no_ptr(model.contract_address);
        model_dict["layout"] = model.layout;

        result.append(model_dict);
    }
    return result;
}
