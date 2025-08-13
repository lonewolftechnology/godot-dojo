#ifndef DOJO_TRANSACTION_QUERY_H
#define DOJO_TRANSACTION_QUERY_H

#include "resources/dojo_query_base.h"
#include "resources/queries/dojo_transaction_filter.h"
#include "dojo_types.h"

using namespace godot;

class DojoTransactionQuery : public DojoQueryBase {
    GDCLASS(DojoTransactionQuery, DojoQueryBase);

    Ref<DojoTransactionFilter> filter;

public:
    DojoTransactionQuery() {}
    ~DojoTransactionQuery() {}

    void* get_native_query() const {
        auto* query = new DOJO::TransactionQuery();

        // Setup pagination
        query->pagination.direction = get_direction();
        
        if (get_limit() > 0) {
            query->pagination.limit.tag = DOJO::Someu32;
            query->pagination.limit.some = get_limit();
        } else {
            query->pagination.limit.tag = DOJO::Noneu32;
        }

        if (!get_cursor().is_empty()) {
            query->pagination.cursor.tag = DOJO::Somec_char;
            char *c_str_cursor = new char[get_cursor().utf8().length() + 1];
            strcpy(c_str_cursor, get_cursor().utf8().get_data());
            query->pagination.cursor.some = c_str_cursor;
        } else {
            query->pagination.cursor.tag = DOJO::Nonec_char;
        }
        
        query->pagination.order_by.data = nullptr;
        query->pagination.order_by.data_len = 0;

        if (filter.is_valid()) {
            query->filter.tag = DOJO::SomeTransactionFilter;
            query->filter.some = filter->get_native();
        } else {
            query->filter.tag = DOJO::NoneTransactionFilter;
        }

        return query;
    }

    void set_filter(const Ref<DojoTransactionFilter>& p_filter) { filter = p_filter; }
    Ref<DojoTransactionFilter> get_filter() const { return filter; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_filter"), &DojoTransactionQuery::get_filter);
        ClassDB::bind_method(D_METHOD("set_filter", "p_filter"), &DojoTransactionQuery::set_filter);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "filter", PROPERTY_HINT_RESOURCE_TYPE, "DojoTransactionFilter"), "set_filter", "get_filter");
    }
};

#endif //DOJO_TRANSACTION_QUERY_H
