//
// Created by hazel on 2/06/25.
//

#ifndef FIELD_ELEMENT_H
#define FIELD_ELEMENT_H

#include "dojo_types.h"

#include "godot_cpp/classes/ref_counted.hpp"

// #include "variant/ty.h"

using namespace godot;

class FieldElement : public RefCounted {
    GDCLASS(FieldElement, RefCounted);

public:
    FieldElement();
    FieldElement(const String& hex_str, size_t max_bytes = 32);
    FieldElement(int enum_value);
    FieldElement(DOJO::FieldElement* existing_felt);
    FieldElement(DOJO::FieldElement existing_felt);
    FieldElement(TypedArray<String>& addresses);
    ~FieldElement();

    static DOJO::FieldElement from_enum(int enum_value);
    static PackedByteArray to_packed_array(const void* data, int size = 32);
    static DOJO::FieldElement from_string(const String& hex_str, size_t max_bytes = 32);
    static dojo_bindings::FieldElement short_string_to_felt(const String& cairo_str);
    static DOJO::FieldElement nulled();
    static PackedByteArray nulled_as_bytes();
    static String nulled_as_string();

    PackedByteArray as_packed_array() const;
    String to_string() const;
    const char* to_string_c_str() const;
    String parse_cairo();
    static std::vector<DOJO::FieldElement> create_array(TypedArray<String> array);
    String bytearray_deserialize();

    static String get_as_string(DOJO::FieldElement* _felt);
    static String get_as_string_no_ptr(dojo_bindings::FieldElement _felt);

    static dojo_bindings::CArrayFieldElement bytearray_serialize(const String& msg);
    static DOJO::FieldElement cairo_short_string_to_felt(const String& msg);

    DOJO::FieldElement* get_felt() const { return felt; }
    DOJO::FieldElement get_felt_no_ptr() const { return *felt; }

protected:
    DOJO::FieldElement* felt = new DOJO::FieldElement();

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("as_packed_array"), &FieldElement::as_packed_array);
        // ClassDB::bind_method(D_METHOD("get"), &FieldElement::as_packed_array);
        // ClassDB::bind_static_method("FieldElement", D_METHOD("from_enum", "p_value"), &FieldElement::from_enum);

        // ClassDB::bind_method(D_METHOD("setup", "p_felt", "p_size"), &FieldElement::from_string);
        // ClassDB::bind_method(D_METHOD("set_from_bytes", "bytes"), &FieldElement::set_from_bytes);
        // ClassDB::bind_method(D_METHOD("to_bytes"), &FieldElement::to_bytes);
        // ClassDB::bind_method(D_METHOD("to_variant"), &FieldElement::to_variant);
        // ClassDB::bind_static_method("FieldElementVariant", D_METHOD("from_variant", "var"), &FieldElement::from_variant);
    }

};



#endif //FIELD_ELEMENT_H
