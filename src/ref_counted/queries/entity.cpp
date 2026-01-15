#include "ref_counted/queries/entity.hpp"

Ref<DojoQuery> DojoQuery::world_addresses(const PackedStringArray& worlds) {
    this->p_world_addresses = worlds;
    return this;
}

Ref<DojoQuery> DojoQuery::add_world(const String& world) {
    this->p_world_addresses.push_back(world);
    return this;
}

Ref<DojoQuery> DojoQuery::hashed_keys(const bool& hashed_keys) {
    this->p_no_hashed_keys = !hashed_keys;
    return this;
}

Ref<DojoQuery> DojoQuery::models(const PackedStringArray& models) {
    this->p_models = models;
    return this;
}

Ref<DojoQuery> DojoQuery::historical(const bool& historical) {
    this->p_historical = historical;
    return this;
}

Ref<DojoQuery> DojoQuery::with_clause(const Ref<DojoClause>& clause) {
    this->p_clause = clause;
    return this;
}

dojo::Query DojoQuery::get_native() const {
    dojo::Query query;

    std::vector<dojo::FieldElement> wa;
    for (int i = 0; i < p_world_addresses.size(); i++) {
        wa.push_back(p_world_addresses[i].utf8().get_data());
    }
    query.world_addresses = wa;

    if (p_clause.is_valid()) {
        query.clause = std::make_shared<dojo::Clause>(p_clause->get_native());
    }

    query.no_hashed_keys = p_no_hashed_keys;

    std::vector<std::string> m;
    for (int i = 0; i < p_models.size(); i++) {
        m.push_back(p_models[i].utf8().get_data());
    }
    query.models = m;

    query.historical = p_historical;

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
