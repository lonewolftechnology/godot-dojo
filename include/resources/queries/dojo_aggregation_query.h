#ifndef DOJO_AGGREGATION_QUERY_H
#define DOJO_AGGREGATION_QUERY_H

#include "resources/dojo_query_base.h"
#include "dojo_types.h"
#include "variant/ty/dojo_array.h"
using namespace godot;

class DojoAggregationQuery : public DojoQueryBase {
    GDCLASS(DojoAggregationQuery, DojoQueryBase);

    TypedArray<String> aggregator_ids;
    TypedArray<String> entity_ids;

public:
    DojoAggregationQuery(){}
    ~DojoAggregationQuery(){}

    void* get_native_query() const {
        auto* query = new DOJO::AggregationQuery();
        query->aggregator_ids = DojoArrayHelpers::string_array_to_native_carray_str(aggregator_ids);
        query->entity_ids = DojoArrayHelpers::string_array_to_native_carray_str(entity_ids);
        query->pagination = get_native_pagination();
        return query;
    }

    void set_aggregator_ids(const TypedArray<String> &p_array) { aggregator_ids = p_array; }
    void set_entity_ids(const TypedArray<String> &p_array) { entity_ids = p_array; }

    TypedArray<String> get_aggregator_ids() { return aggregator_ids; }
    TypedArray<String> get_entity_ids() { return entity_ids; }


protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_aggregator_ids", "p_array"), &DojoAggregationQuery::set_aggregator_ids);
        ClassDB::bind_method(D_METHOD("get_aggregator_ids"), &DojoAggregationQuery::get_aggregator_ids);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "aggregator_ids", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_aggregator_ids", "get_aggregator_ids");

        ClassDB::bind_method(D_METHOD("set_entity_ids", "p_array"), &DojoAggregationQuery::set_entity_ids);
        ClassDB::bind_method(D_METHOD("get_entity_ids"), &DojoAggregationQuery::get_entity_ids);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "entity_ids", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_entity_ids", "get_entity_ids");

    }
};


#endif //DOJO_AGGREGATION_QUERY_H
