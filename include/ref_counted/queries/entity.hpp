#pragma once

#include "ref_counted/query_builder.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"
#include "dojo/dojo.hpp"

class DojoQuery : public QueryBuilder {
    GDCLASS(DojoQuery, QueryBuilder)

    // Query (entities)
    PackedStringArray p_world_addresses{};
    Ref<DojoClause> p_clause{}; // Option
    bool p_no_hashed_keys = false;
    PackedStringArray p_models{};
    bool p_historical = false;

public:
    DojoQuery() : QueryBuilder(Entity){}

    Ref<DojoQuery> world_addresses(const PackedStringArray& worlds);
    Ref<DojoQuery> add_world(const String& world);
    Ref<DojoQuery> hashed_keys(const bool& hashed_keys);
    Ref<DojoQuery> models(const PackedStringArray& models);
    Ref<DojoQuery> historical(const bool& historical);
    Ref<DojoQuery> with_clause(const Ref<DojoClause>& clause);

    dojo::Query get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("world_addresses", "worlds"), &DojoQuery::world_addresses);
        ClassDB::bind_method(D_METHOD("add_world", "world"), &DojoQuery::add_world);
        ClassDB::bind_method(D_METHOD("hashed_keys", "hashed_keys"), &DojoQuery::hashed_keys);
        ClassDB::bind_method(D_METHOD("models", "models"), &DojoQuery::models);
        ClassDB::bind_method(D_METHOD("historical", "historical"), &DojoQuery::historical);
        ClassDB::bind_method(D_METHOD("with_clause", "clause"), &DojoQuery::with_clause);
    }
};
