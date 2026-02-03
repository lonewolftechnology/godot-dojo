#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class ActivityQuery : public QueryBuilder {
    GDCLASS(ActivityQuery, QueryBuilder)

    PackedStringArray p_world_addresses{};
    PackedStringArray p_namespaces{};
    PackedStringArray p_caller_addresses{};
    uint64_t p_from_time = 0; // Option
    uint64_t p_to_time = 0; // Option

public:
    ActivityQuery() : QueryBuilder(Activity){}

    Ref<ActivityQuery> world_addresses(const PackedStringArray& worlds);
    Ref<ActivityQuery> namespaces(const PackedStringArray& namespaces);
    Ref<ActivityQuery> caller_addresses(const PackedStringArray& callers);
    Ref<ActivityQuery> from_time(const uint64_t& time);
    Ref<ActivityQuery> to_time(const uint64_t& time);

    dojo::ActivityQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("world_addresses", "worlds"), &ActivityQuery::world_addresses);
        ClassDB::bind_method(D_METHOD("namespaces", "namespaces"), &ActivityQuery::namespaces);
        ClassDB::bind_method(D_METHOD("caller_addresses", "callers"), &ActivityQuery::caller_addresses);
        ClassDB::bind_method(D_METHOD("from_time", "time"), &ActivityQuery::from_time);
        ClassDB::bind_method(D_METHOD("to_time", "time"), &ActivityQuery::to_time);
    }
};
