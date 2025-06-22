// De:
// Created by hazel on 27/05/25.
// A:
// Created by hazel on 27/05/25.

#include "../../include/variant/primitive.h"

#include "debug_macros.h"

Variant DojoPrimitive::VariantFromPrimitive(dojo_bindings::Primitive primitive)
{
    switch (primitive.tag)
    {
    case DOJO::Primitive_Tag::I8:
        return {primitive.i8};
        break;
    case DOJO::Primitive_Tag::I16:
    break;
        return {primitive.i16};
    case DOJO::Primitive_Tag::I32:
        return {primitive.i32};
        break;
    case DOJO::Primitive_Tag::I64:
        return {primitive.i64};
        break;
    case DOJO::Primitive_Tag::I128:
        // uint8_t* i128_data = primitive.i128;
        return FieldElement::to_packed_array(primitive.i128,16);
        // return {Variant(i128_data)};
        break;
    case DOJO::Primitive_Tag::U8:
        return {primitive.u8};
        break;
    case DOJO::Primitive_Tag::U16:
        return {primitive.u16};
        break;
    case DOJO::Primitive_Tag::U32:
        return {primitive.u32};
        break;
    case DOJO::Primitive_Tag::U64:
        return {primitive.u64};
        break;
    case DOJO::Primitive_Tag::U128:
        // uint8_t* u128_data = primitive.u128;
        return FieldElement::to_packed_array(primitive.u128,16);
        // return {Variant(u128_data)};
        break;
    case DOJO::Primitive_Tag::U256_:
        // uint8_t* data = primitive.u256.data;
        return FieldElement::to_packed_array(primitive.u256.data,32);
        // return {Variant(data)};
        break;
    case DOJO::Primitive_Tag::Bool:
        return {primitive.bool_};
        break;
    default:
        return {};
        break;
    }
    return {};

}

FieldElement DojoPrimitive::FieldElementFromPrimitive(DOJO::Primitive primitive)
{
    switch (primitive.tag)
    {
    case DOJO::Primitive_Tag::Felt252:
        return {primitive.felt252};
    case DOJO::Primitive_Tag::ClassHash:
        return {primitive.class_hash};
    case DOJO::Primitive_Tag::ContractAddress:
        return {primitive.contract_address};
    case DOJO::Primitive_Tag::EthAddress:
        return {primitive.eth_address};
    default:
        return {};
    }
    return {};
}


void DojoPrimitive::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_value"), &DojoPrimitive::get_value);
    ClassDB::bind_method(D_METHOD("set_value", "value"), &DojoPrimitive::set_value);
    // ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
}

DojoPrimitive::DojoPrimitive()
{
    LOG_DEBUG("Primitive constructor called");
}

DojoPrimitive::DojoPrimitive(const DOJO::Primitive& primitive)
{
    switch (primitive.tag)
    {
    case DOJO::Primitive_Tag::I8:
    case DOJO::Primitive_Tag::I16:
    case DOJO::Primitive_Tag::I32:
    case DOJO::Primitive_Tag::I64:
    case DOJO::Primitive_Tag::I128:
    case DOJO::Primitive_Tag::U8:
    case DOJO::Primitive_Tag::U16:
    case DOJO::Primitive_Tag::U32:
    case DOJO::Primitive_Tag::U64:
    case DOJO::Primitive_Tag::U128:
    case DOJO::Primitive_Tag::U256_:
    case DOJO::Primitive_Tag::Bool:
        value = VariantFromPrimitive(primitive);
        break;
    case DOJO::Primitive_Tag::Felt252:
    case DOJO::Primitive_Tag::ClassHash:
    case DOJO::Primitive_Tag::ContractAddress:
    case DOJO::Primitive_Tag::EthAddress:
        FieldElement data = FieldElementFromPrimitive(primitive);
        felt = &data;
        is_felt = true;
        break;
    }
}
