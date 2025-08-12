//
// Created by hazel on 1/08/25.
//

#ifndef DOJO_OPTION_H
#define DOJO_OPTION_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

class DojoOption : public Resource {
    GDCLASS(DojoOption, Resource);

protected:
    Variant _value;

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("is_some"), &DojoOption::is_some);
        ClassDB::bind_method(D_METHOD("get_value"), &DojoOption::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "p_value"), &DojoOption::set_value);
    }

public:
    DojoOption() : _value(Variant()) {}
    DojoOption(const Variant& p_value) : _value(p_value) {}

    bool is_some() const {
        return _value.get_type() != Variant::NIL;
    }

    void set_value(const Variant& p_value) {
        _value = p_value;
    }

    Variant get_value() const {
        return _value;
    }
};

#endif //DOJO_OPTION_H
