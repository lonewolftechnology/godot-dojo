#include "ref_counted/queries/player_achievement.hpp"

Ref<PlayerAchievementQuery> PlayerAchievementQuery::world_addresses(const PackedStringArray& worlds) {
    this->p_world_addresses = worlds;
    return this;
}

Ref<PlayerAchievementQuery> PlayerAchievementQuery::namespaces(const PackedStringArray& namespaces) {
    this->p_namespaces = namespaces;
    return this;
}

Ref<PlayerAchievementQuery> PlayerAchievementQuery::player_addresses(const PackedStringArray& players) {
    this->p_player_addresses = players;
    return this;
}

dojo::PlayerAchievementQuery PlayerAchievementQuery::get_native() const {
    dojo::PlayerAchievementQuery query;

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

    std::vector<dojo::FieldElement> pa;
    for (int i = 0; i < p_player_addresses.size(); i++) {
        pa.push_back(p_player_addresses[i].utf8().get_data());
    }
    query.player_addresses = pa;

    auto pagination = get_native_pagination();
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
