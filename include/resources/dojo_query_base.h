//
// Created by hazel on 7/8/25.
//

#ifndef DOJO_QUERY_BASE_H
#define DOJO_QUERY_BASE_H
#include "godot_cpp/core/class_db.hpp"

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "dojo_types.h"
#include "dojo_order_by.h"
using namespace  godot;
class DojoQueryBase : public Resource
{
    GDCLASS(DojoQueryBase, Resource);
    
    uint32_t limit;
    String cursor;
    DOJO::PaginationDirection direction;
    TypedArray<DeprecatedOrderBy> order_by;

public:
    DojoQueryBase()
    {
        limit = 10;
        direction = DOJO::PaginationDirection::Forward;
    };
    ~DojoQueryBase(){};

    DOJO::Pagination get_native_pagination() const {
        DOJO::Pagination pagination{};

        pagination.limit.tag = DOJO::COptionu32_Tag::Someu32;
        pagination.limit.some = limit;

        if (cursor.is_empty()) {
            pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
            pagination.cursor.some = nullptr;
        } else {
            pagination.cursor.tag = DOJO::COptionc_char_Tag::Somec_char;
            CharString utf8_cursor = cursor.utf8();
            char *c_str_cursor = (char*) memalloc(utf8_cursor.length() + 1);
            strcpy(c_str_cursor, utf8_cursor.get_data());
            pagination.cursor.some = c_str_cursor;
        }

        pagination.direction = direction;

        pagination.order_by.data_len = order_by.size();
        if (pagination.order_by.data_len > 0) {
            pagination.order_by.data = (dojo_bindings::OrderBy*) memalloc(sizeof(dojo_bindings::OrderBy) * pagination.order_by.data_len);
            for (int i = 0; i < pagination.order_by.data_len; ++i) {
                Ref<DeprecatedOrderBy> ob = order_by[i];
                pagination.order_by.data[i] = ob->get_native_order_by();
            }
        }
        return pagination;
    }

    void set_limit(uint32_t p_limit) { limit = p_limit; }
    uint32_t get_limit() const { return limit; }

    void set_cursor(const String& p_cursor) { cursor = p_cursor; }
    String get_cursor() const { return cursor; }

    DOJO::PaginationDirection get_direction() const {return direction;}
    void set_direction(DOJO::PaginationDirection p_direction) {direction = p_direction;}

    void set_order_by(const TypedArray<DeprecatedOrderBy>& p_order_by) { order_by = p_order_by; }
    TypedArray<DeprecatedOrderBy> get_order_by() const { return order_by; }

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_limit"), &DojoQueryBase::get_limit);
        ClassDB::bind_method(D_METHOD("set_limit", "p_limit"), &DojoQueryBase::set_limit);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "limit"), "set_limit", "get_limit");

        ClassDB::bind_method(D_METHOD("get_cursor"), &DojoQueryBase::get_cursor);
        ClassDB::bind_method(D_METHOD("set_cursor", "p_cursor"), &DojoQueryBase::set_cursor);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "cursor"), "set_cursor", "get_cursor");
        
        BIND_ENUM_CONSTANT(DOJO::PaginationDirection::Forward);
        BIND_ENUM_CONSTANT(DOJO::PaginationDirection::Backward);

        ClassDB::bind_method(D_METHOD("get_direction"), &DojoQueryBase::get_direction);
        ClassDB::bind_method(D_METHOD("set_direction", "p_direction"), &DojoQueryBase::set_direction);
        ADD_PROPERTY(PropertyInfo(
                         Variant::INT,
                         "direction",
                         PROPERTY_HINT_ENUM,
                         "Forward,Backward"
                     ),
                     "set_direction",
                     "get_direction"
        );

        ClassDB::bind_method(D_METHOD("get_order_by"), &DojoQueryBase::get_order_by);
        ClassDB::bind_method(D_METHOD("set_order_by", "p_order_by"), &DojoQueryBase::set_order_by);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "order_by", PROPERTY_HINT_ARRAY_TYPE, "DojoOrderBy"), "set_order_by", "get_order_by");

    };
};
VARIANT_ENUM_CAST(DOJO::PaginationDirection);

#endif //DOJO_QUERY_BASE_H