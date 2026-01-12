 #pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "order_by.hpp"

using namespace godot;

class DojoPagination : public RefCounted {
    GDCLASS(DojoPagination, RefCounted)
    String p_cursor;
    int64_t p_limit = 0;
    int64_t p_direction = static_cast<int64_t>(dojo::PaginationDirection::kForward);
    // Ref<DojoOrderBy> p_order_by;
    TypedArray<DojoOrderBy> p_order_by;

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
        p_order_by.append(order_by);
        return this;
    }

    dojo::Pagination get_native() {
        dojo::Pagination pagination;

        if (!p_cursor.is_empty()) {
            pagination.cursor = p_cursor.utf8().get_data();
        }

        if (p_limit > 0) {
            pagination.limit = static_cast<uint32_t>(p_limit);
        }

        pagination.direction = static_cast<dojo::PaginationDirection>(p_direction);

        for (int i = 0; i < p_order_by.size(); i++) {
            Ref<DojoOrderBy> order_by = p_order_by[i];
            if (order_by.is_valid()) {
                pagination.order_by.push_back(std::make_shared<dojo::OrderBy>(order_by->get_native()));
            }
        }

        return pagination;
    }

    Dictionary to_dict() {
        Dictionary _pagination = {};

        _pagination["cursor"] = p_cursor;
        _pagination["limit"] = p_limit;
        _pagination["direction"] = p_direction;

        Array order_by_array;
        for (int i = 0; i < p_order_by.size(); i++) {
            Ref<DojoOrderBy> order_by = p_order_by[i];
            if (order_by.is_valid()) {
                order_by_array.append(order_by->to_dict());
            }
        }
        _pagination["order_by"] = order_by_array;

        Dictionary result = {};
        result["pagination"] = _pagination;

        return result;
    }

protected:
    static void _bind_methods() {
        // PaginationDirection
        ClassDB::bind_integer_constant(get_class_static(), "PaginationDirection", "Forward",
                                       static_cast<GDExtensionInt>(dojo::PaginationDirection::kForward));
        ClassDB::bind_integer_constant(get_class_static(), "PaginationDirection", "Backward",
                                       static_cast<GDExtensionInt>(dojo::PaginationDirection::kBackward));

        ClassDB::bind_method(D_METHOD("cursor", "cursor"), &DojoPagination::cursor);
        ClassDB::bind_method(D_METHOD("limit", "limit"), &DojoPagination::limit);
        ClassDB::bind_method(D_METHOD("direction", "direction"), &DojoPagination::direction);
        ClassDB::bind_method(D_METHOD("order_by", "order_by"), &DojoPagination::order_by);
        ClassDB::bind_method(D_METHOD("to_dict"), &DojoPagination::to_dict);
    }
};
