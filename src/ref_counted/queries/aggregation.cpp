#include "ref_counted/queries/aggregation.hpp"

Ref<AggregationQuery> AggregationQuery::aggregator_ids(const PackedStringArray& ids) {
    this->p_aggregator_ids = ids;
    return this;
}

Ref<AggregationQuery> AggregationQuery::entity_ids(const PackedStringArray& ids) {
    this->p_entity_ids = ids;
    return this;
}

#ifndef WEB_ENABLED
dojo::AggregationQuery AggregationQuery::get_native() const {
    dojo::AggregationQuery query;

    std::vector<std::string> ai;
    for (int i = 0; i < p_aggregator_ids.size(); i++) {
        ai.push_back(p_aggregator_ids[i].utf8().get_data());
    }
    query.aggregator_ids = ai;

    std::vector<std::string> ei;
    for (int i = 0; i < p_entity_ids.size(); i++) {
        ei.push_back(p_entity_ids[i].utf8().get_data());
    }
    query.entity_ids = ei;

    auto pagination = get_native_pagination();
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
#endif

Dictionary AggregationQuery::to_dict() const {
    Dictionary result = QueryBuilder::to_dict();
    result["aggregator_ids"] = p_aggregator_ids;
    result["entity_ids"] = p_entity_ids;
    return result;
}
