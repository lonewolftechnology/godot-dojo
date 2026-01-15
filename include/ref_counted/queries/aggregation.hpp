#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class AggregationQuery : public QueryBuilder {
    GDCLASS(AggregationQuery, QueryBuilder)

    PackedStringArray p_aggregator_ids{};
    PackedStringArray p_entity_ids{};

public:
    AggregationQuery() : QueryBuilder(Aggregation){}

    Ref<AggregationQuery> aggregator_ids(const PackedStringArray& ids);
    Ref<AggregationQuery> entity_ids(const PackedStringArray& ids);

    dojo::AggregationQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("aggregator_ids", "ids"), &AggregationQuery::aggregator_ids);
        ClassDB::bind_method(D_METHOD("entity_ids", "ids"), &AggregationQuery::entity_ids);
    }
};
