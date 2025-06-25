//
// Created by hazel on 24/06/25.
//

#ifndef ARRAY_TY_H
#define ARRAY_TY_H
#include "godot_cpp/classes/ref_counted.hpp"

#include "dojo_types.h"
using namespace godot;

class ArrayDojoTy : public RefCounted
{
    GDCLASS(ArrayDojoTy, RefCounted);

    String name = "";
    Variant value = Variant::NIL;

public:
    ArrayDojoTy();
    ~ArrayDojoTy();
    ArrayDojoTy(DOJO::CArrayTy array_ty);

    Variant ArrayDojoTyToVariant(DOJO::CArrayTy array_ty);

    Variant get_value() const {return value;}
    void set_value(const Variant& p_value) { value = p_value; }
    String get_name() const {return name;}
    void set_name(const String& p_name) {name = p_name;}

protected:
    static void _bind_methods()
    {

    };
};

#endif //ARRAY_TY_H
