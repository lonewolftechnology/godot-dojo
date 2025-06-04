//
// Created by hazel on 2/06/25.
//

#ifndef TY_H
#define TY_H
#include <godot_cpp/godot.hpp>
#include <dojo_types.h>

#include "classes/ref_counted.hpp"
#include "variant/variant.hpp"

using namespace godot;


class DojoTy : public RefCounted
{
    GDCLASS(DojoTy, RefCounted);

private:
    Variant value = Variant::NIL;

public:
    DojoTy();
    ~DojoTy();
    DojoTy(const dojo_bindings::Ty &ty);

    void set_value(const Variant &p_value) { value = p_value; }

    Variant get_value() const { return value; }



protected:
    static void _bind_methods();
};

#endif //TY_H
