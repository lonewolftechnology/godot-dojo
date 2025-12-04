#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "variant/ty/dojo_array.h"
using namespace godot;

class DojoOrderBy : public RefCounted {
    GDCLASS(DojoOrderBy, RefCounted)
    TypedArray<Dictionary> p_order_by;

public:
    DojoOrderBy() {}

    ~DojoOrderBy() {}

    Ref<DojoOrderBy> add(const String &field, const int64_t &direction) {
        Dictionary _dict = {};
        _dict["field"] = field;
        _dict["direction"] = direction;
        this->p_order_by.push_back(_dict);
        return this;
    }

    Ref<DojoOrderBy> set(const TypedArray<Dictionary> &order_by) {
        this->p_order_by = order_by;
        return this;
    }

    DOJO::CArrayOrderBy get_native() {
        return DojoArrayHelpers::dictionary_array_to_native_carray_orderby(p_order_by);
    }

    String _to_string() {
        return JSON::stringify(p_order_by, "\t");
    }

    Dictionary to_dict() {
        Dictionary result;
        result["order_by"] = p_order_by;
        return result;
    }

protected:
    static void _bind_methods() {
        // OrderDirection
        ClassDB::bind_integer_constant(get_class_static(), "OrderDirection", "Asc", DOJO::OrderDirection::Asc);
        ClassDB::bind_integer_constant(get_class_static(), "OrderDirection", "Desc", DOJO::OrderDirection::Desc);

        // Methods
        ClassDB::bind_method(D_METHOD("add", "field", "direction"), &DojoOrderBy::add);
        ClassDB::bind_method(D_METHOD("set", "order_by"), &DojoOrderBy::set);
        ClassDB::bind_method(D_METHOD("to_dict"), &DojoOrderBy::to_dict);
        ClassDB::bind_method(D_METHOD("to_string"), &DojoOrderBy::_to_string);
    }
};