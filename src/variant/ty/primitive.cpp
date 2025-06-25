// De:
// Created by hazel on 27/05/25.
// A:
// Created by hazel on 27/05/25.

#include "variant/ty/primitive.h"

Variant DojoPrimitive::VariantFromPrimitive(dojo_bindings::Primitive primitive)
{
    switch (primitive.tag)
    {
    case DOJO::Primitive_Tag::I8:
        return {primitive.i8};
    case DOJO::Primitive_Tag::I16:
        return {primitive.i16};
    case DOJO::Primitive_Tag::I32:
        return {primitive.i32};
    case DOJO::Primitive_Tag::I64:
        return {primitive.i64};
    case DOJO::Primitive_Tag::I128:
        return FieldElement::to_packed_array(primitive.i128, 16);
    case DOJO::Primitive_Tag::U8:
        return {primitive.u8};
    case DOJO::Primitive_Tag::U16:
        return {primitive.u16};
    case DOJO::Primitive_Tag::U32:
        return {primitive.u32};
    case DOJO::Primitive_Tag::U64:
        return {primitive.u64};
    case DOJO::Primitive_Tag::U128:
        return FieldElement::to_packed_array(primitive.u128, 16);
    case DOJO::Primitive_Tag::U256_:
        return FieldElement::to_packed_array(primitive.u256.data, 32);
    case DOJO::Primitive_Tag::Bool:
        return {primitive.bool_};
    default:
        return {};
    }
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
}

DojoPrimitive::DojoPrimitive()
{
    Logger::debug("Primitive constructor called");
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
        value = data.to_string();
        is_felt = true;
        break;
    }
}
