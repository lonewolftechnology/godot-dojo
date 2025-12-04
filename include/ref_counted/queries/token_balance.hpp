#pragma once

#include "ref_counted/query_builder.hpp"

class TokenBalanceQuery : public QueryBuilder {
    GDCLASS(TokenBalanceQuery, QueryBuilder)

public:
    TokenBalanceQuery() : QueryBuilder(TokenBalance){}

protected:
    void static _bind_methods() {
    }
};