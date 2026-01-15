#include "ref_counted/queries/aggregation.hpp"

Ref<AggregationQuery> AggregationQuery::aggregator_ids(const PackedStringArray& ids) {
    this->p_aggregator_ids = ids;
    return this;
}

Ref<AggregationQuery> AggregationQuery::entity_ids(const PackedStringArray& ids) {
    this->p_entity_ids = ids;
    return this;
}

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

    auto pagination = std::make_shared<dojo::Pagination>();
    if (get_limit() > 0) {
        pagination->limit = get_limit();
    }
    if (!get_cursor().is_empty()) {
        pagination->cursor = get_cursor().utf8().get_data();
    }
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
