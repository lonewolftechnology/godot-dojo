#pragma once

#include "ref_counted/query_builder.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"
// TODO: Refactor: EntityQuery -> Query
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
    Ref<EntityQuery> with_clause(const Ref<DojoClause>& clause);

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("world_addresses", "worlds"), &EntityQuery::world_addresses);
        ClassDB::bind_method(D_METHOD("add_world", "world"), &EntityQuery::add_world);
        ClassDB::bind_method(D_METHOD("hashed_keys", "hashed_keys"), &EntityQuery::hashed_keys);
        ClassDB::bind_method(D_METHOD("models", "models"), &EntityQuery::models);
        ClassDB::bind_method(D_METHOD("historical", "historical"), &EntityQuery::historical);
        ClassDB::bind_method(D_METHOD("with_clause", "clause"), &EntityQuery::with_clause);
    }
};
