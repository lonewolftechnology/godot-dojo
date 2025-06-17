//
// Created by hazel on 27/05/25.
//

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "dojo_types.hpp"

using namespace godot;

class DojoPrimitive : public RefCounted {
    GDCLASS(DojoPrimitive, RefCounted);

using Tag = DOJO::Primitive::Tag;

private:
    Variant value = Variant::NIL;

protected:
    static void _bind_methods();

public:
    static const char* PrimitiveTagToString(int value);
    DojoPrimitive();
    DojoPrimitive(const DOJO::Primitive &primitive);

    Variant get_value() const { return value; }
    void set_value(const Variant &p_value) { value = p_value; }
};

#endif //PRIMITIVE_H
