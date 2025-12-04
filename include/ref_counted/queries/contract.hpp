#pragma once

#include "ref_counted/query_builder.hpp"

class ContractQuery : public QueryBuilder {
    GDCLASS(ContractQuery, QueryBuilder)

public:
    ContractQuery() : QueryBuilder(Contract){}

protected:
    void static _bind_methods() {
    }
};