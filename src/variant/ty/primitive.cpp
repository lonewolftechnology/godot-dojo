//
// Created by hazel on 27/05/25.
//

#include "variant/ty/primitive.h"

#include <types/big_int.h>

#include "tools/dojo_helper.h"

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
        {
            Ref<I128> val = memnew(I128(primitive.i128));
            return val;
        }
    case DOJO::Primitive_Tag::U8:
        return {primitive.u8};
    case DOJO::Primitive_Tag::U16:
        return {primitive.u16};
    case DOJO::Primitive_Tag::U32:
        return {primitive.u32};
    case DOJO::Primitive_Tag::U64:
        return {primitive.u64};
    case DOJO::Primitive_Tag::U128:
        {
            Ref<U128> val = memnew(U128(primitive.u128));
            return val;
        }
    case DOJO::Primitive_Tag::U256_:
        {
            Ref<U256> val = memnew(U256(primitive.u256));
            return val;
        }
    case DOJO::Primitive_Tag::Bool:
        return {primitive.bool_};
    default:
        return {};
    }
}

String DojoPrimitive::FieldElementFromPrimitive(DOJO::Primitive primitive)
{
    DOJO::FieldElement felt;
    switch (primitive.tag)
    {
    case DOJO::Primitive_Tag::Felt252:
        felt = primitive.felt252;
        break;
    case DOJO::Primitive_Tag::ClassHash:
        felt = primitive.class_hash;
        break;
    case DOJO::Primitive_Tag::ContractAddress:
        felt = primitive.contract_address;
        break;
    case DOJO::Primitive_Tag::EthAddress:
        felt = primitive.eth_address;
        break;
    default: ;
    }
    return FieldElement::get_as_string(&felt);
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
        value = FieldElementFromPrimitive(primitive);
        is_felt = true;
        break;
    }
    Logger::debug_extra("Primitive", value);
}
