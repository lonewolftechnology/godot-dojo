#pragma once

#include "ref_counted/query_builder.hpp"
#include "ref_counted/dojo_utilities/clauses/keys.hpp"
#include "dojo/dojo.hpp"

class EventQuery : public QueryBuilder {
    GDCLASS(EventQuery, QueryBuilder)

    Ref<KeysClause> p_keys{}; // Option

public:
    EventQuery() : QueryBuilder(Event){}

    Ref<EventQuery> keys(const Ref<KeysClause>& keys);

    dojo::EventQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("keys", "keys"), &EventQuery::keys);
    }
};
