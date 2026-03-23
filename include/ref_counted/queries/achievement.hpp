#pragma once

#include "ref_counted/query_builder.hpp"

class AchievementQuery : public QueryBuilder {
    GDCLASS(AchievementQuery, QueryBuilder)

    PackedStringArray p_world_addresses{};
    PackedStringArray p_namespaces{};
    bool p_hidden = false; // Option

public:
    AchievementQuery() : QueryBuilder(Achievement){}

    Ref<AchievementQuery> world_addresses(const PackedStringArray& worlds);
    Ref<AchievementQuery> namespaces(const PackedStringArray& namespaces);
    Ref<AchievementQuery> hidden(const bool& hidden);
    virtual Dictionary to_dict() const override;
#ifndef WEB_ENABLED
    dojo::AchievementQuery get_native() const;
#endif

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("world_addresses", "worlds"), &AchievementQuery::world_addresses);
        ClassDB::bind_method(D_METHOD("namespaces", "namespaces"), &AchievementQuery::namespaces);
        ClassDB::bind_method(D_METHOD("hidden", "hidden"), &AchievementQuery::hidden);
    }
};
