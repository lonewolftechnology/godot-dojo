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

String DojoPrimitive::TagToString(DOJO::Primitive_Tag tag)
{
    switch (tag)
    {
    case DOJO::Primitive_Tag::I8: return "I8";
    case DOJO::Primitive_Tag::I16: return "I16";
    case DOJO::Primitive_Tag::I32: return "I32";
    case DOJO::Primitive_Tag::I64: return "I64";
    case DOJO::Primitive_Tag::I128: return "I128";
    case DOJO::Primitive_Tag::U8: return "U8";
    case DOJO::Primitive_Tag::U16: return "U16";
    case DOJO::Primitive_Tag::U32: return "U32";
    case DOJO::Primitive_Tag::U64: return "U64";
    case DOJO::Primitive_Tag::U128: return "U128";
    case DOJO::Primitive_Tag::U256_: return "U256";
    case DOJO::Primitive_Tag::Bool: return "Bool";
    case DOJO::Primitive_Tag::Felt252: return "Felt252";
    case DOJO::Primitive_Tag::ClassHash: return "ClassHash";
    case DOJO::Primitive_Tag::ContractAddress: return "ContractAddress";
    case DOJO::Primitive_Tag::EthAddress: return "EthAddress";
    default:
        return "Unknown";
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
        value = FieldElementFromPrimitive(primitive);
        is_felt = true;
        break;
    }
    Logger::success_extra("Primitive", value);
    Logger::debug_extra("Primitive Tag", TagToString(primitive.tag));
}
