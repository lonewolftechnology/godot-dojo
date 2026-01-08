//
// Created by hazel on 24/06/25.
//

#ifndef ENUM_H
#define ENUM_H
#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;
class DojoEnum : public RefCounted
{
    GDCLASS(DojoEnum, RefCounted);

    Variant value = Variant::NIL;
    String name;
    String string_option;

public:
    DojoEnum();
    DojoEnum(const DOJO::Enum &enum_);
    ~DojoEnum();

    Variant VariantFromEnum(const dojo_bindings::Enum& enum_);

    Variant get_value() const {return value;}
    void set_value(const Variant& p_value) { value = p_value; }
    String get_name() const {return name;}
    void set_name(const String& p_name) {name = p_name;}

    String get_enum_type() const {return name;}
    String to_string() const {return string_option;}
    int get_option() const {return value;}

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_value"), &DojoEnum::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "value"), &DojoEnum::set_value);
        ClassDB::bind_method(D_METHOD("get_name"), &DojoEnum::get_name);
        ClassDB::bind_method(D_METHOD("set_name", "name"), &DojoEnum::set_name);

        ClassDB::bind_method(D_METHOD("get_option"), &DojoEnum::get_option);
        ClassDB::bind_method(D_METHOD("to_string"), &DojoEnum::to_string);
    };
};

#endif //ENUM_H
