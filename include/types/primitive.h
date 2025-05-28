//
// Created by hazel on 27/05/25.
//

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <godot_cpp/classes/ref_counted.hpp>
#include "dojo_types.h"
#include "variant/variant.hpp"

using namespace godot;

class DojoPrimitive : public RefCounted {
    GDCLASS(DojoPrimitive, RefCounted)

using Tag = dojo_bindings::Primitive_Tag;

private:
    Variant value = Variant::NIL;

protected:
    static void _bind_methods();

public:
    DojoPrimitive();
    DojoPrimitive(const dojo_bindings::Primitive &primitive);
    DojoPrimitive(const dojo_bindings::FieldElement field_element);

    Variant get_value() const { return value; }
    void set_value(const Variant &p_value) { value = p_value; }
};

#endif //PRIMITIVE_H
