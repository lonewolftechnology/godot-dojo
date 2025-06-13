//
// Created by hazel on 3/06/25.
//
#include <variant/field_element.h>

#include "debug_macros.h"

using namespace godot;

FieldElement::FieldElement()
= default;

FieldElement::FieldElement(const String& hex_str, size_t max_bytes)
{
    // const std::string& hex_str{hex_str_godot.utf8()};
    // Skip "0x" prefix if present
    size_t start_idx = (hex_str.substr(0, 2) == "0x") ? 2 : 0;

    // Calculate actual string length without prefix
    size_t hex_length = hex_str.length() - start_idx;

    // Handle odd number of characters by assuming leading zero
    bool is_odd = hex_length % 2 != 0;
    size_t num_bytes = (hex_length + is_odd) / 2;

    // Ensure we don't overflow the output buffer
    if (num_bytes > max_bytes)
    {
        return;
        //        throw std::runtime_error("Input hex string too long for output buffer");
    }

    size_t out_idx = 0;

    // Handle first nibble separately if we have odd number of characters
    if (is_odd)
    {
        String nibble = hex_str.substr(start_idx, 1);
        felt->data[out_idx++] = static_cast<uint8_t>(nibble.hex_to_int());
    }

    // Process two hex digits at a time
    for (size_t i = is_odd ? 1 : 0; i < hex_length; i += 2)
    {
        String byte_str = hex_str.substr(start_idx + i, 2);
        felt->data[out_idx++] = static_cast<uint8_t>(byte_str.hex_to_int());
    }
}

FieldElement::FieldElement(int enum_value)
{
    memset(felt->data, 0, 32);
    felt->data[31] = static_cast<uint8_t>(enum_value);
}

FieldElement::FieldElement(dojo_bindings::FieldElement* existing_felt)
{
    felt = existing_felt;
}

FieldElement::~FieldElement()
{
}

PackedByteArray FieldElement::to_packed_array(const void* data, const int size)
{
    PackedByteArray _bytes;
    _bytes.resize(size);
    memcpy(_bytes.ptrw(), data, size);
    LOG_INFO("[FieldElement] -> [PackedByteArray] ", _bytes.hex_encode());
    return _bytes;
}

PackedByteArray FieldElement::as_packed_array() const
{
    return to_packed_array(felt->data);
}

String FieldElement::to_string() const
{
    String ret = "0x";
    for (size_t i = 0; i < 32; i++)
    {
        ret += String::num_int64(felt->data[i], 16, false);
    };

    return ret;
}

const char* FieldElement::to_string_c_str() const
{
    return to_string().utf8().get_data();
}

Ref<FieldElement> FieldElement::from_enum(int enum_value)
{
    Ref<FieldElement> field_element;
    field_element.instantiate();
    // Debug: Verificar estado del objeto
    if (field_element.is_null())
    {
        LOG_ERROR("Failed to instantiate FieldElement");
        return {};
    }

    if (field_element->felt == nullptr)
    {
        LOG_ERROR("felt is null after instantiation");
        // Limpiar e inicializar
        field_element->felt = new dojo_bindings::FieldElement();
    }
    memset(field_element->felt->data, 0, 32);
    field_element->felt->data[31] = static_cast<uint8_t>(enum_value);


    return field_element;
}

String FieldElement::bytearray_deserialize()
{
    dojo_bindings::Resultc_char testing = dojo_bindings::bytearray_deserialize(get_felt(), 32);
    if (testing.tag == dojo_bindings::Resultc_char_Tag::Errc_char)
    {
        LOG_DEBUG("Can't deserialize... Trying Cairo String");
        return parse_cairo();
    }
    else
    {
        LOG_SUCCESS("Felt:", testing.ok);
    }
    return {testing.ok};
}

String FieldElement::get_as_string(dojo_bindings::FieldElement* _felt)
{
    String ret = "0x";
    for (size_t i = 0; i < 32; i++)
    {
        ret += String::num_int64(_felt->data[i], 16, false);
    };

    return ret;
}

String FieldElement::parse_cairo()
{
    dojo_bindings::Resultc_char testing = dojo_bindings::parse_cairo_short_string(get_felt_no_ptr());
    if (testing.tag == dojo_bindings::Resultc_char_Tag::Errc_char)
    {
        LOG_DEBUG("No cairo string found, returning as string hex");
        return {to_string()};
    }
    else
    {
        LOG_SUCCESS("Cairo String:", testing.ok);
    }
    return {testing.ok};
}
