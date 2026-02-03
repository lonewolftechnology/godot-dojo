#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class TokenContractQuery : public QueryBuilder {
    GDCLASS(TokenContractQuery, QueryBuilder)

    PackedStringArray p_contract_addresses{};
    TypedArray<int> p_contract_types{};

public:
    TokenContractQuery() : QueryBuilder(TokenContract){}

    Ref<TokenContractQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<TokenContractQuery> contract_types(const TypedArray<int>& types);

    dojo::TokenContractQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &TokenContractQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("contract_types", "types"), &TokenContractQuery::contract_types);
    }
};
