//
// Created by hazel on 7/8/25.
//

#ifndef DOJO_ORDER_BY_H
#define DOJO_ORDER_BY_H

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "dojo_types.h"

using namespace godot;

class DojoOrderBy : public Resource {
    GDCLASS(DojoOrderBy, Resource);

    String field;
    DOJO::OrderDirection direction;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_field"), &DojoOrderBy::get_field);
        ClassDB::bind_method(D_METHOD("set_field", "p_field"), &DojoOrderBy::set_field);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "field"), "set_field", "get_field");

        BIND_ENUM_CONSTANT(DOJO::OrderDirection::Asc);
        BIND_ENUM_CONSTANT(DOJO::OrderDirection::Desc);

        ClassDB::bind_method(D_METHOD("get_direction"), &DojoOrderBy::get_direction);
        ClassDB::bind_method(D_METHOD("set_direction", "p_direction"), &DojoOrderBy::set_direction);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "direction", PROPERTY_HINT_ENUM, "Asc,Desc"), "set_direction", "get_direction");
    }

public:
    DojoOrderBy() : direction(DOJO::OrderDirection::Asc) {}
    ~DojoOrderBy() {}

    void set_field(const String& p_field) { field = p_field; }
    String get_field() const { return field; }

    void set_direction(DOJO::OrderDirection p_direction) { direction = p_direction; }
    DOJO::OrderDirection get_direction() const { return direction; }
};

VARIANT_ENUM_CAST(DOJO::OrderDirection);

#endif //DOJO_ORDER_BY_H