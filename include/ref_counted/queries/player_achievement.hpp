#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class PlayerAchievementQuery : public QueryBuilder {
    GDCLASS(PlayerAchievementQuery, QueryBuilder)

    PackedStringArray p_world_addresses{};
    PackedStringArray p_namespaces{};
    PackedStringArray p_player_addresses{};

public:
    PlayerAchievementQuery() : QueryBuilder(PlayerAchievement){}

    Ref<PlayerAchievementQuery> world_addresses(const PackedStringArray& worlds);
    Ref<PlayerAchievementQuery> namespaces(const PackedStringArray& namespaces);
    Ref<PlayerAchievementQuery> player_addresses(const PackedStringArray& players);

    dojo::PlayerAchievementQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("world_addresses", "worlds"), &PlayerAchievementQuery::world_addresses);
        ClassDB::bind_method(D_METHOD("namespaces", "namespaces"), &PlayerAchievementQuery::namespaces);
        ClassDB::bind_method(D_METHOD("player_addresses", "players"), &PlayerAchievementQuery::player_addresses);
    }
};
