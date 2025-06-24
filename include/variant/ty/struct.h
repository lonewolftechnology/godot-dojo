//
// Created by hazel on 24/06/25.
//

#ifndef STRUCT_H
#define STRUCT_H
#include "godot_cpp/classes/ref_counted.hpp"
#include "dojo_types.h"
using namespace godot;

class DojoStruct : public RefCounted
{
    GDCLASS(DojoStruct, RefCounted);
    String name = "";
    Variant value = Variant::NIL;
    bool is_felt = false;

public:
    DojoStruct(){};
    DojoStruct(DOJO::Struct struct_);
    DojoStruct(DOJO::Member member);
    ~DojoStruct(){};

    Variant StructToVariant(DOJO::Struct struct_);
    Variant get_value() const {return value;}
    void set_value(const Variant& p_value) { value = p_value; }
    String get_name() const {return name;}
    void set_name(const String& p_name) {name = p_name;}



protected:

    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_value"), &DojoStruct::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "value"), &DojoStruct::set_value);
        ClassDB::bind_method(D_METHOD("get_name"), &DojoStruct::get_name);
        ClassDB::bind_method(D_METHOD("set_name", "name"), &DojoStruct::set_name);
        // ADD_PROPERTY(PropertyInfo(Variant::INT, "value"), "set_value", "get_value");
    };
};

#endif //STRUCT_H
