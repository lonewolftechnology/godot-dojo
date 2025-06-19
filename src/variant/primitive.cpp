// De:
// Created by hazel on 27/05/25.
// A:
// Created by hazel on 27/05/25.

#include "../../include/variant/primitive.h"

#include "debug_macros.h"

Variant DojoPrimitive::VariantFromPrimitive(dojo_bindings::Primitive primitive)
{
    if (primitive.IsI8())
    {
        return {GET_DOJO_VALUE(primitive.i8)};
    }
    if (primitive.IsI16())
    {
        return {GET_DOJO_VALUE(primitive.i16)};
    }
    if (primitive.IsI32())
    {
        return {GET_DOJO_VALUE(primitive.i32)};
    }
    if (primitive.IsI64())
    {
        return {GET_DOJO_VALUE(primitive.i64)};
    }
    if (primitive.IsI128())
    {
        uint8_t* data = GET_DOJO_VALUE(primitive.i128);
        // return FieldElement::to_packed_array(data,16);
        return {Variant(data)};
    }
    if (primitive.IsU8())
    {
        return {GET_DOJO_VALUE(primitive.u8)};
    }
    if (primitive.IsU16())
    {
        return {GET_DOJO_VALUE(primitive.u16)};
    }
    if (primitive.IsU32())
    {
        return {GET_DOJO_VALUE(primitive.u32)};
    }
    if (primitive.IsU64())
    {
        return {GET_DOJO_VALUE(primitive.u64)};
    }
    if (primitive.IsU128())
    {
        uint8_t* data = GET_DOJO_VALUE(primitive.u128);
        // return FieldElement::to_packed_array(data,16);
        return {Variant(data)};
    }
    if (primitive.IsU256_())
    {
        uint8_t* data = GET_DOJO_VALUE(primitive.u256).data;
        return FieldElement::to_packed_array(data);
    }
    if (primitive.IsBool())
    {
        return {GET_DOJO_VALUE(primitive.bool_)};
    }
    return {};
}

FieldElement DojoPrimitive::FieldElementFromPrimitive(DOJO::Primitive primitive)
{
    if (primitive.IsFelt252())
    {
        return {GET_DOJO_VALUE(primitive.felt252)};
    }
    if (primitive.IsClassHash())
    {
        return {GET_DOJO_VALUE(primitive.class_hash)};
    }
    if (primitive.IsContractAddress())
    {
        return {GET_DOJO_VALUE(primitive.contract_address)};
    }
    if (primitive.IsEthAddress())
    {
        return {GET_DOJO_VALUE(primitive.eth_address)};
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
    case DOJO::Primitive::Tag::I8:
    case DOJO::Primitive::Tag::I16:
    case DOJO::Primitive::Tag::I32:
    case DOJO::Primitive::Tag::I64:
    case DOJO::Primitive::Tag::I128:
    case DOJO::Primitive::Tag::U8:
    case DOJO::Primitive::Tag::U16:
    case DOJO::Primitive::Tag::U32:
    case DOJO::Primitive::Tag::U64:
    case DOJO::Primitive::Tag::U128:
    case DOJO::Primitive::Tag::U256_:
    case DOJO::Primitive::Tag::Bool:
        value = VariantFromPrimitive(primitive);
        break;
    case DOJO::Primitive::Tag::Felt252:
    case DOJO::Primitive::Tag::ClassHash:
    case DOJO::Primitive::Tag::ContractAddress:
    case DOJO::Primitive::Tag::EthAddress:
        FieldElement data = FieldElementFromPrimitive(primitive);
        felt = &data;
        is_felt = true;
        break;
    }
}
