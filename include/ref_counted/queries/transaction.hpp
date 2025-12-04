#pragma once

#include "ref_counted/query_builder.hpp"

class TransactionQuery : public QueryBuilder {
    GDCLASS(TransactionQuery, QueryBuilder)

public:
    TransactionQuery() : QueryBuilder(Transaction){}

protected:
    void static _bind_methods() {
    }
};