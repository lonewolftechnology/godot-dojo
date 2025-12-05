#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class MemberValue : public RefCounted {
    GDCLASS(MemberValue, RefCounted)
    DOJO::MemberValue type;
    Variant p_value; // MemberValue: Primitive, String, MemberArray

public:
    MemberValue() {}
    ~MemberValue(){}

    MemberValue(const Variant &value) : p_value(value) {
        switch (value.get_type()) {
            case Variant::STRING:
                type = DOJO::MemberValue_Tag::String;
                break;
        }
    }
};
