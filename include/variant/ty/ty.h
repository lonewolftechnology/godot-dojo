//
// Created by hazel on 2/06/25.
//

#ifndef TY_H
#define TY_H

#include "dojo_types.h"

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "tools/logger.h"
#include "variant/field_element.h"
#include "primitive.h"

using namespace godot;

class DojoTy : public RefCounted
{
    GDCLASS(DojoTy, RefCounted);
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

    Variant value = Variant::NIL;
    Type type = Type::NIL;
    String name = {};

public:
    bool is_felt = false;
    DojoTy();
    ~DojoTy();
    DojoTy(const DOJO::Ty &ty);
    DojoTy(const DOJO::Ty* ty);
    DojoTy(const DOJO::Member &member);
    DojoTy(const DOJO::CArrayTy &array_ty);
    // Ty(const DOJO::Ty* ty);

    void init_form_ty(const DOJO::Ty &ty);

    void set_value(const Variant &p_value) { value = p_value; }
    //
    Variant get_value() const
    {
        return value;
    }
    FieldElement get_felt() const
    {
        if (is_felt)
        {
            return {value.stringify()};
        }else
        {
            Logger::warning("No felt found, returning empty one");
            return {};
        }
    }

    String get_name() const { return name; }
    void set_name(const String &p_name) { name = p_name; }

    //
    // String get_type() const { return type; }
    // void set_type(const String &p_type) { type = p_type; }





protected:
    static void _bind_methods();
};
#endif //TY_H
