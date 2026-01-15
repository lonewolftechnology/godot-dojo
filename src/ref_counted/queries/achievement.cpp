#include "ref_counted/queries/achievement.hpp"

Ref<AchievementQuery> AchievementQuery::world_addresses(const PackedStringArray& worlds) {
    this->p_world_addresses = worlds;
    return this;
}

Ref<AchievementQuery> AchievementQuery::namespaces(const PackedStringArray& namespaces) {
    this->p_namespaces = namespaces;
    return this;
}

Ref<AchievementQuery> AchievementQuery::hidden(const bool& hidden) {
    this->p_hidden = hidden;
    return this;
}

dojo::AchievementQuery AchievementQuery::get_native() const {
    dojo::AchievementQuery query;

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

    query.hidden = p_hidden;

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
