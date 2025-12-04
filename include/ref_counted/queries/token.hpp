#pragma once

#include "ref_counted/query_builder.hpp"

class TokenQuery : public QueryBuilder {
    GDCLASS(TokenQuery, QueryBuilder)

public:
    TokenQuery() : QueryBuilder(Token){}

protected:
    void static _bind_methods() {
    }
};