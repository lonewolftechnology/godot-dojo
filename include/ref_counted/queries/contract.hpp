#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class ContractQuery : public QueryBuilder {
    GDCLASS(ContractQuery, QueryBuilder)

    PackedStringArray p_contract_addresses{};
    TypedArray<int> p_contract_types{};

public:
    ContractQuery() : QueryBuilder(Contract){}

    Ref<ContractQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<ContractQuery> contract_types(const TypedArray<int>& types);

    dojo::ContractQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &ContractQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("contract_types", "types"), &ContractQuery::contract_types);
    }
};
