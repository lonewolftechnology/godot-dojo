//
// Created by chozo on 07-10-25.
//

#ifndef AGGREGATION_H
#define AGGREGATION_H
#include "resources/dojo_subscription.h"
#include "dojo_types.h"
#include "variant/ty/dojo_array.h"

class AggregationSubscription: public DojoSubscription {

    GDCLASS(AggregationSubscription, DojoSubscription)

    TypedArray<String> aggregator_ids;
    TypedArray<String> entity_ids;

public:
    AggregationSubscription(){}
    ~AggregationSubscription(){}

    Type get_type() const override {
        return AGGREGATION;
    }

    void set_aggregator_ids(const TypedArray<String>& p_array){ aggregator_ids = p_array; }
    void set_entity_ids(const TypedArray<String>& p_array){ entity_ids = p_array; }

    TypedArray<String> get_aggregator_ids(){ return aggregator_ids; }
    TypedArray<String> get_entity_ids(){ return entity_ids; }

    // The DOJO::CArrayc_char is used as a helper tool is already implemented.
    // Its structure satisfied both parameters for the subscription
    DOJO::CArrayc_char get_native_aggregator_id() const {
        return DojoArrayHelpers::string_array_to_native_carray_str(aggregator_ids);
    }

    DOJO::CArrayc_char get_native_entity_ids() const {
        return DojoArrayHelpers::string_array_to_native_carray_str(entity_ids);
    }


protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_aggregator_ids", "p_array"), &AggregationSubscription::set_aggregator_ids);
        ClassDB::bind_method(D_METHOD("get_aggregator_ids"), &AggregationSubscription::get_aggregator_ids);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "aggregator_ids"), "set_aggregator_ids", "get_aggregator_ids");

        ClassDB::bind_method(D_METHOD("set_entity_ids", "p_array"), &AggregationSubscription::set_entity_ids);
        ClassDB::bind_method(D_METHOD("get_entity_ids"), &AggregationSubscription::get_entity_ids);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "entity_ids"), "set_entity_ids", "get_entity_ids");


    }
};

#endif //AGGREGATION_H
