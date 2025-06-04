//
// Created by hazel on 2/06/25.
//

#ifndef FIELD_ELEMENT_H
#define FIELD_ELEMENT_H

#include <dojo_types.h>

#include <godot_cpp/classes/ref_counted.hpp>

// #include <variant/ty.h>

using namespace godot;

class FieldElement : public RefCounted {
    GDCLASS(FieldElement, RefCounted);

private:
    dojo_bindings::FieldElement* field_element;

public:
    FieldElement();
    FieldElement(const String& hex_str, size_t max_bytes = 32);
    ~FieldElement();

    static PackedByteArray to_packed_array(const void* data, int size = 32);

    PackedByteArray as_packed_array() const;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("as_packed_array"), &FieldElement::as_packed_array);
        ClassDB::bind_method(D_METHOD("get"), &FieldElement::as_packed_array);
        // ClassDB::bind_method(D_METHOD("setup", "p_felt", "p_size"), &FieldElement::from_string);
        // ClassDB::bind_method(D_METHOD("set_from_bytes", "bytes"), &FieldElement::set_from_bytes);
        // ClassDB::bind_method(D_METHOD("to_bytes"), &FieldElement::to_bytes);
        // ClassDB::bind_method(D_METHOD("to_variant"), &FieldElement::to_variant);
        // ClassDB::bind_static_method("FieldElementVariant", D_METHOD("from_variant", "var"), &FieldElement::from_variant);
    }
};


#endif //FIELD_ELEMENT_H
