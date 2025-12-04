#pragma once

#include "ref_counted/query_builder.hpp"

class TokenTransferQuery : public QueryBuilder {
    GDCLASS(TokenTransferQuery, QueryBuilder)

public:
    TokenTransferQuery() : QueryBuilder(TokenTransfer){}

protected:
    void static _bind_methods() {
    }
};