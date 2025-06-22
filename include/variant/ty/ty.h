//
// Created by hazel on 2/06/25.
//

#ifndef TY_H
#define TY_H

#include "dojo_types.h"

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "debug_macros.h"
#include "variant/field_element.h"
#include "variant/primitive.h"

using namespace godot;

class Ty : public RefCounted
{
    GDCLASS(Ty, RefCounted);
    friend class Variant;

    using Tag = DOJO::Ty_Tag;

    enum class Type {
        Primitive_,
        Struct_,
        Enum_,
        Tuple_,
        Array_,
        ByteArray,
        NIL
      };

private:
    Variant value = Variant::NIL;
    DojoPrimitive* primitive = {};
    Type type = Type::NIL;
    String name = {};

public:
    Ty();
    ~Ty();
    Ty(const DOJO::Ty &ty);
    Ty(const DOJO::Member& member);
    // Ty(const DOJO::Ty* ty);

    void set_value(const Variant &p_value) { value = p_value; }
    //
    Variant get_value() const
    {
        if (type == Type::Primitive_)
        {
            return primitive->get_value();
        }
        return value;
    }
    //
    // String get_type() const { return type; }
    // void set_type(const String &p_type) { type = p_type; }





protected:
    static void _bind_methods();
};
#endif //TY_H
