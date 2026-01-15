#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class TokenQuery : public QueryBuilder {
    GDCLASS(TokenQuery, QueryBuilder)

    PackedStringArray p_contract_addresses{};
    PackedStringArray p_token_ids{};

    struct AttributeFilter {
        String trait_name;
        String trait_value;
    };
    std::vector<AttributeFilter> p_attribute_filters;

public:
    TokenQuery() : QueryBuilder(Token){}

    Ref<TokenQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<TokenQuery> token_ids(const PackedStringArray& ids);
    Ref<TokenQuery> attribute_filter(const String& trait_name, const String& trait_value);

    dojo::TokenQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &TokenQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("token_ids", "ids"), &TokenQuery::token_ids);
        ClassDB::bind_method(D_METHOD("attribute_filter", "trait_name", "trait_value"), &TokenQuery::attribute_filter);
    }
};
