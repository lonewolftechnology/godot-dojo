#pragma once

#include "ref_counted/query_builder.hpp"
#include "../dojo_utilities/clause.hpp"

class EntityQuery : public QueryBuilder {
    GDCLASS(EntityQuery, QueryBuilder)

    // Query (entities)
    PackedStringArray p_world_addresses{};
    Ref<DojoClause> p_clause{}; // Option
    bool p_no_hashed_keys = true;
    PackedStringArray p_models{};
    bool p_historical = true;

public:
    EntityQuery() : QueryBuilder(Entity){}

    Ref<EntityQuery> world_addresses(const PackedStringArray& worlds);
    Ref<EntityQuery> add_world(const String& world);
    Ref<EntityQuery> hashed_keys(const bool& hashed_keys);
    Ref<EntityQuery> models(const PackedStringArray& models);
    Ref<EntityQuery> historical(const bool& historical);

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("world_addresses", "worlds"), &EntityQuery::world_addresses);
    }
};
