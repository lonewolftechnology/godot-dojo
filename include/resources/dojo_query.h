//
// Created by hazel on 11/07/25.
//

#ifndef DOJO_QUERY_H
#define DOJO_QUERY_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"

using namespace godot;

class DojoQuery : public Resource
{
    GDCLASS(DojoQuery, Resource);

private:
    int limit = 10;
    String cursor;
    String direction = "forward";
    Array order_by;
    Dictionary clause;
    bool no_hashed_keys = false;
    Array models; // Array of strings
    bool historical = false;

public:
    DojoQuery() {};
    ~DojoQuery() {};

    // Limit
    int get_limit() const { return limit; }
    void set_limit(int p_limit)
    {
        limit = p_limit;
        emit_changed();
    }

    // Cursor
    String get_cursor() const { return cursor; }
    void set_cursor(const String& p_cursor)
    {
        cursor = p_cursor;
        emit_changed();
    }

    // Direction
    String get_direction() const { return direction; }
    void set_direction(const String& p_direction)
    {
        direction = p_direction;
        emit_changed();
    }

    // Order By
    Array get_order_by() const { return order_by; }
    void set_order_by(const Array& p_order_by)
    {
        order_by = p_order_by;
        emit_changed();
    }

    // Clause
    Dictionary get_clause() const { return clause; }
    void set_clause(const Dictionary& p_clause)
    {
        clause = p_clause;
        emit_changed();
    }

    // No Hashed Keys
    bool get_no_hashed_keys() const { return no_hashed_keys; }
    void set_no_hashed_keys(bool p_no_hashed_keys)
    {
        no_hashed_keys = p_no_hashed_keys;
        emit_changed();
    }

    // Models
    Array get_models() const { return models; }
    void set_models(const Array& p_models)
    {
        models = p_models;
        emit_changed();
    }

    // Historical
    bool get_historical() const { return historical; }
    void set_historical(bool p_historical)
    {
        historical = p_historical;
        emit_changed();
    }

protected:
    static void _bind_methods()
    {
        // Limit
        ClassDB::bind_method(D_METHOD("get_limit"), &DojoQuery::get_limit);
        ClassDB::bind_method(D_METHOD("set_limit", "limit"), &DojoQuery::set_limit);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "limit"), "set_limit", "get_limit");

        // Cursor
        ClassDB::bind_method(D_METHOD("get_cursor"), &DojoQuery::get_cursor);
        ClassDB::bind_method(D_METHOD("set_cursor", "cursor"), &DojoQuery::set_cursor);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "cursor"), "set_cursor", "get_cursor");

        // Direction
        ClassDB::bind_method(D_METHOD("get_direction"), &DojoQuery::get_direction);
        ClassDB::bind_method(D_METHOD("set_direction", "direction"), &DojoQuery::set_direction);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "direction", PROPERTY_HINT_ENUM, "forward,backward"), "set_direction",
                     "get_direction");

        // Order By
        ClassDB::bind_method(D_METHOD("get_order_by"), &DojoQuery::get_order_by);
        ClassDB::bind_method(D_METHOD("set_order_by", "order_by"), &DojoQuery::set_order_by);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "order_by"), "set_order_by", "get_order_by");

        // Clause
        ClassDB::bind_method(D_METHOD("get_clause"), &DojoQuery::get_clause);
        ClassDB::bind_method(D_METHOD("set_clause", "clause"), &DojoQuery::set_clause);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "clause"), "set_clause", "get_clause");

        // No Hashed Keys
        ClassDB::bind_method(D_METHOD("get_no_hashed_keys"), &DojoQuery::get_no_hashed_keys);
        ClassDB::bind_method(D_METHOD("set_no_hashed_keys", "no_hashed_keys"), &DojoQuery::set_no_hashed_keys);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "no_hashed_keys"), "set_no_hashed_keys", "get_no_hashed_keys");

        // Models
        ClassDB::bind_method(D_METHOD("get_models"), &DojoQuery::get_models);
        ClassDB::bind_method(D_METHOD("set_models", "models"), &DojoQuery::set_models);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "models", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_models",
                     "get_models");

        // Historical
        ClassDB::bind_method(D_METHOD("get_historical"), &DojoQuery::get_historical);
        ClassDB::bind_method(D_METHOD("set_historical", "historical"), &DojoQuery::set_historical);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "historical"), "set_historical", "get_historical");
    }
};

#endif //DOJO_QUERY_H