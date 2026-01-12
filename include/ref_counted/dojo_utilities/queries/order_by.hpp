#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "variant/ty/dojo_array.h"
using namespace godot;

class DojoOrderBy : public RefCounted
{
    GDCLASS(DojoOrderBy, RefCounted)
    String field;
    dojo::OrderDirection direction;

public:
    DojoOrderBy()
    {
    }

    ~DojoOrderBy()
    {
    }

    Ref<DojoOrderBy> set(const String& _field, const int64_t& _direction)
    {
        this->field = _field;
        this->direction = static_cast<dojo::OrderDirection>(_direction);
        return this;
    }

    dojo::OrderBy get_native()
    {
        dojo::OrderBy order_by = {};
        order_by.field = field.utf8().get_data();
        order_by.direction = direction;
        return order_by;
    }

    String _to_string()
    {
        return JSON::stringify(to_dict(), "\t");
    }

    Dictionary to_dict()
    {
        Dictionary result;
        result["field"] = field;
        result["direction"] = static_cast<int64_t>(direction);
        return result;
    }

protected:
    static void _bind_methods()
    {
        // OrderDirection
        ClassDB::bind_integer_constant(get_class_static(), "OrderDirection", "Asc",
                                       static_cast<GDExtensionInt>(dojo::OrderDirection::kAsc));
        ClassDB::bind_integer_constant(get_class_static(), "OrderDirection", "Desc",
                                       static_cast<GDExtensionInt>(dojo::OrderDirection::kDesc));

        // Methods
        ClassDB::bind_method(D_METHOD("set", "order_by"), &DojoOrderBy::set);
        ClassDB::bind_method(D_METHOD("to_dict"), &DojoOrderBy::to_dict);
        ClassDB::bind_method(D_METHOD("to_string"), &DojoOrderBy::_to_string);
    }
};
