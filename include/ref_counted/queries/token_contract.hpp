#pragma once

#include "ref_counted/query_builder.hpp"

class TokenContractQuery : public QueryBuilder {
    GDCLASS(TokenContractQuery, QueryBuilder)

public:
    TokenContractQuery() : QueryBuilder(TokenContract){}

protected:
    void static _bind_methods() {
    }
};