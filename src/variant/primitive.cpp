// De:
// Created by hazel on 27/05/25.
// A:
// Created by hazel on 27/05/25.

#include "../../include/variant/primitive.h"

#include "debug_macros.h"
#include "variant/utility_functions.hpp"
// Esto despues se borra, es para debugguear más fácil falsos positivos/bugs
const char* DojoPrimitive::PrimitiveTagToString(int value)
{
    static const char* names[] = {
        "I8",
        "I16",
        "I32",
        "I64",
        "I128",
        "U8",
        "U16",
        "U32",
        "U64",
        "U128",
        "U256",
        "Bool",
        "Felt252",
        "ClassHash",
        "ContractAddress",
        "EthAddress"
    };

    if (value >= 0 && value < std::size(names))
    {
        return names[value];
    }
    return "Unknown";
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

PackedByteArray DataArrayToPackedByteArray(const void* data, const int size = 32)
{
    PackedByteArray _bytes;
    _bytes.resize(size);
    memcpy(_bytes.ptrw(), data, size);
    return _bytes;
}

DojoPrimitive::DojoPrimitive(const dojo_bindings::Primitive& primitive)
{
    switch (primitive.tag)
    {
    case Tag::I8:
        value = Variant(primitive.i8);
        break;
    case Tag::I16:
        value = Variant(primitive.i16);
        break;
    case Tag::I32:
        value = Variant(primitive.i32);
        break;
    case Tag::I64:
        value = Variant(primitive.i64);
        break;
    case Tag::I128:
        value = Variant(primitive.i128);
        break;
    case Tag::U8:
        value = Variant(primitive.u8);
        break;
    case Tag::U16:
        value = Variant(primitive.u16);
        break;
    case Tag::U32:
        value = Variant(primitive.u32);
        break;
    case Tag::U64:
        value = Variant(primitive.u64);
        break;
    case Tag::U128:
        value = DataArrayToPackedByteArray(primitive.u128, 16);
        break;
    case Tag::U256_:
        value = DataArrayToPackedByteArray(primitive.u256.data);
        break;
    case Tag::Bool:
        value = Variant(primitive.bool_);
        break;
    case Tag::Felt252:
        LOG_DEBUG("FELT");
        value = DataArrayToPackedByteArray(primitive.felt252.data);
        break;
    case Tag::ClassHash:
        LOG_DEBUG("CLASHASH");
        value = DataArrayToPackedByteArray(primitive.class_hash.data);
        break;
    case Tag::ContractAddress:
        LOG_DEBUG("CONTRACT");
        value = DataArrayToPackedByteArray(primitive.contract_address.data);
        break;
    case Tag::EthAddress:
        LOG_DEBUG("ETH");
        value = DataArrayToPackedByteArray(primitive.eth_address.data);
        break;
    default:
        UtilityFunctions::push_error("Primitive ", PrimitiveTagToString(primitive.tag), " doesn't have a Constructor");
        break;
    }
    if (value.get_type() != Variant::NIL)
    {
        UtilityFunctions::print_rich("[color=Cyan]Primitive Constructor Concluded");
        UtilityFunctions::print_rich(
            "[color=Yellow]Primitive " + String(value) + "\n[color=Green]Type: " + Variant::get_type_name(
                value.get_type()));
    }
}
