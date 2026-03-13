#pragma once

#include "ref_counted/query_builder.hpp"

class ControllerQuery : public QueryBuilder {
    GDCLASS(ControllerQuery, QueryBuilder)

    PackedStringArray p_contract_addresses{};
    PackedStringArray p_usernames{};

public:
    ControllerQuery() : QueryBuilder(Controller){}

    Ref<ControllerQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<ControllerQuery> usernames(const PackedStringArray& usernames);

#ifndef WEB_ENABLED
    dojo::ControllerQuery get_native() const;
#endif

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &ControllerQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("usernames", "usernames"), &ControllerQuery::usernames);
    }
};
