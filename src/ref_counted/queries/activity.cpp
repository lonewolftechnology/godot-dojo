#include "ref_counted/queries/activity.hpp"

Ref<ActivityQuery> ActivityQuery::world_addresses(const PackedStringArray& worlds) {
    this->p_world_addresses = worlds;
    return this;
}

Ref<ActivityQuery> ActivityQuery::namespaces(const PackedStringArray& namespaces) {
    this->p_namespaces = namespaces;
    return this;
}

Ref<ActivityQuery> ActivityQuery::caller_addresses(const PackedStringArray& callers) {
    this->p_caller_addresses = callers;
    return this;
}

Ref<ActivityQuery> ActivityQuery::from_time(const uint64_t& time) {
    this->p_from_time = time;
    return this;
}

Ref<ActivityQuery> ActivityQuery::to_time(const uint64_t& time) {
    this->p_to_time = time;
    return this;
}

dojo::ActivityQuery ActivityQuery::get_native() const {
    dojo::ActivityQuery query;

    std::vector<dojo::FieldElement> wa;
    for (int i = 0; i < p_world_addresses.size(); i++) {
        wa.push_back(p_world_addresses[i].utf8().get_data());
    }
    query.world_addresses = wa;

    std::vector<std::string> ns;
    for (int i = 0; i < p_namespaces.size(); i++) {
        ns.push_back(p_namespaces[i].utf8().get_data());
    }
    query.namespaces = ns;

    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < p_caller_addresses.size(); i++) {
        ca.push_back(p_caller_addresses[i].utf8().get_data());
    }
    query.caller_addresses = ca;

    if (p_from_time > 0) query.from_time = p_from_time;
    if (p_to_time > 0) query.to_time = p_to_time;

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
