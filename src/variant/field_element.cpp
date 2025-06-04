//
// Created by hazel on 3/06/25.
//
#include <variant/field_element.h>

#include "debug_macros.h"

using namespace godot;

FieldElement::FieldElement()
{
}

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
        field_element->data[out_idx++] = static_cast<uint8_t>(nibble.hex_to_int());
    }

    // Process two hex digits at a time
    for (size_t i = is_odd ? 1 : 0; i < hex_length; i += 2)
    {
        String byte_str = hex_str.substr(start_idx + i, 2);
        field_element->data[out_idx++] = static_cast<uint8_t>(byte_str.hex_to_int());
    }
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
    return to_packed_array(field_element->data);
}
