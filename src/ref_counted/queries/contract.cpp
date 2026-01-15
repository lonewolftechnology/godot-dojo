#include "ref_counted/queries/contract.hpp"

Ref<ContractQuery> ContractQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<ContractQuery> ContractQuery::contract_types(const TypedArray<int>& types) {
    this->p_contract_types = types;
    return this;
}

dojo::ContractQuery ContractQuery::get_native() const {
    dojo::ContractQuery query;

    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < p_contract_addresses.size(); i++) {
        ca.push_back(p_contract_addresses[i].utf8().get_data());
    }
    query.contract_addresses = ca;

    std::vector<dojo::ContractType> ct;
    for (int i = 0; i < p_contract_types.size(); i++) {
        ct.push_back(static_cast<dojo::ContractType>((int)p_contract_types[i]));
    }
    query.contract_types = ct;

    return query;
}
