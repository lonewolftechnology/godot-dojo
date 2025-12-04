#pragma once

#include "ref_counted/query_builder.hpp"

class AggregationQuery : public QueryBuilder {
    GDCLASS(AggregationQuery, QueryBuilder)

public:
    AggregationQuery() : QueryBuilder(Aggregation){}

protected:
    void static _bind_methods() {
    }
};