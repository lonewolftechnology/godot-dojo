#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class TokenTransferQuery : public QueryBuilder {
    GDCLASS(TokenTransferQuery, QueryBuilder)

    PackedStringArray p_contract_addresses{};
    PackedStringArray p_account_addresses{};
    PackedStringArray p_token_ids{};

public:
    TokenTransferQuery() : QueryBuilder(TokenTransfer){}

    Ref<TokenTransferQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<TokenTransferQuery> account_addresses(const PackedStringArray& addresses);
    Ref<TokenTransferQuery> token_ids(const PackedStringArray& ids);

    dojo::TokenTransferQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &TokenTransferQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("account_addresses", "addresses"), &TokenTransferQuery::account_addresses);
        ClassDB::bind_method(D_METHOD("token_ids", "ids"), &TokenTransferQuery::token_ids);
    }
};
