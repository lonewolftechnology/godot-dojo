#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "order_by.hpp"

using namespace godot;

class DojoPagination : public RefCounted {
    GDCLASS(DojoPagination, RefCounted)
    String p_cursor;
    int64_t p_limit = 0;
    int64_t p_direction = DOJO::PaginationDirection::Forward;
    Ref<DojoOrderBy> p_order_by;

public:
    DojoPagination() {
    };

    ~DojoPagination() {
    };

    Ref<DojoPagination> cursor(const String &cursor) {
        this->p_cursor = cursor;
        return this;
    }

    Ref<DojoPagination> limit(const int64_t &limit) {
        this->p_limit = limit;
        return this;
    }

    Ref<DojoPagination> direction(const int64_t &direction) {
        this->p_direction = direction;
        return this;
    }

    Ref<DojoPagination> order_by(const Ref<DojoOrderBy> &order_by) {
        this->p_order_by = order_by;
        return this;
    }

    DOJO::Pagination get_native() {
        DOJO::Pagination pagination = {};

        pagination.cursor.tag = p_cursor.is_empty() ? DOJO::Nonec_char : DOJO::Somec_char;
        if (pagination.cursor.tag == DOJO::Somec_char) {
            pagination.cursor.some = strdup(p_cursor.utf8().get_data());
        }

        pagination.limit.tag = p_limit > 0 ? DOJO::Someu32 : DOJO::Noneu32;
        if (pagination.limit.tag == DOJO::Someu32) {
            pagination.limit.some = p_limit;
        }

        pagination.direction = static_cast<DOJO::PaginationDirection>(p_direction);

        if (p_order_by.is_valid()) {
            pagination.order_by = p_order_by->get_native();
        }

        return pagination;
    }

    Dictionary to_dict() {
        Dictionary _pagination = {};

        _pagination["cursor"] = p_cursor;
        _pagination["limit"] = p_limit;
        _pagination["direction"] = p_direction;
        _pagination.merge(p_order_by->to_dict());
        Dictionary result = {};
        result["pagination"] = _pagination;

        return result;
    }

protected:
    static void _bind_methods() {
        // PaginationDirection
        ClassDB::bind_integer_constant(get_class_static(), "PaginationDirection", "Forward",
                                       DOJO::PaginationDirection::Forward);
        ClassDB::bind_integer_constant(get_class_static(), "PaginationDirection", "Backward",
                                       DOJO::PaginationDirection::Backward);

        ClassDB::bind_method(D_METHOD("cursor", "cursor"), &DojoPagination::cursor);
        ClassDB::bind_method(D_METHOD("limit", "limit"), &DojoPagination::limit);
        ClassDB::bind_method(D_METHOD("direction", "direction"), &DojoPagination::direction);
        ClassDB::bind_method(D_METHOD("order_by", "order_by"), &DojoPagination::order_by);
        ClassDB::bind_method(D_METHOD("to_dict"), &DojoPagination::to_dict);
    }
};
