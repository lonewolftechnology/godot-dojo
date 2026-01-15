#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class TokenBalanceQuery : public QueryBuilder {
    GDCLASS(TokenBalanceQuery, QueryBuilder)

    PackedStringArray p_contract_addresses{};
    PackedStringArray p_account_addresses{};
    PackedStringArray p_token_ids{};

public:
    TokenBalanceQuery() : QueryBuilder(TokenBalance){}

    Ref<TokenBalanceQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<TokenBalanceQuery> account_addresses(const PackedStringArray& addresses);
    Ref<TokenBalanceQuery> token_ids(const PackedStringArray& ids);

    dojo::TokenBalanceQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &TokenBalanceQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("account_addresses", "addresses"), &TokenBalanceQuery::account_addresses);
        ClassDB::bind_method(D_METHOD("token_ids", "ids"), &TokenBalanceQuery::token_ids);
    }
};
