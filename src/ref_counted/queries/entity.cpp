#include "ref_counted/queries/entity.hpp"

Ref<EntityQuery> EntityQuery::world_addresses(const PackedStringArray& worlds) {
    this->p_world_addresses = worlds;
    return this;
}

Ref<EntityQuery> EntityQuery::add_world(const String& world) {
    this->p_world_addresses.push_back(world);
    return this;
}

Ref<EntityQuery> EntityQuery::hashed_keys(const bool& hashed_keys) {
    this->p_no_hashed_keys = !hashed_keys;
    return this;
}

Ref<EntityQuery> EntityQuery::models(const PackedStringArray& models) {
    this->p_models = models;
    return this;
}

Ref<EntityQuery> EntityQuery::historical(const bool& historical) {
    this->p_historical = historical;
    return this;
}

Ref<EntityQuery> EntityQuery::with_clause(const Ref<DojoClause>& clause) {
    this->p_clause = clause;
    return this;
}