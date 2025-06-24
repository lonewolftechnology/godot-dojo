//
// Created by hazel on 27/05/25.
//

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "variant/field_element.h"
#include "tools/logger.h"
using namespace godot;

class DojoPrimitive : public RefCounted
{
    GDCLASS(DojoPrimitive, RefCounted);

    using Tag = DOJO::Primitive_Tag;

    Variant value = Variant::NIL;

public:
    bool is_felt = false;

    static Variant VariantFromPrimitive(DOJO::Primitive primitive);
    static FieldElement FieldElementFromPrimitive(DOJO::Primitive primitive);
    DojoPrimitive();
    DojoPrimitive(const DOJO::Primitive& primitive);

    Variant get_value() const {return value;}

    void set_value(const Variant& p_value) { value = p_value; }
    FieldElement get_felt() const
    {
        if (is_felt)
        {
            return {value.stringify()};
        }else
        {
            LOG_WARNING("No felt found, returning empty one");
            return {};
        }
    }

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_value"), &DojoPrimitive::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "value"), &DojoPrimitive::set_value);
        // ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
    }
};

#endif //PRIMITIVE_H
