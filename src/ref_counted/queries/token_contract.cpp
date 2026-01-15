#include "ref_counted/queries/token_contract.hpp"

Ref<TokenContractQuery> TokenContractQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<TokenContractQuery> TokenContractQuery::contract_types(const TypedArray<int>& types) {
    this->p_contract_types = types;
    return this;
}

dojo::TokenContractQuery TokenContractQuery::get_native() const {
    dojo::TokenContractQuery query;

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

    auto pagination = std::make_shared<dojo::Pagination>();
    if (get_limit() > 0) {
        pagination->limit = get_limit();
    }
    if (!get_cursor().is_empty()) {
        pagination->cursor = get_cursor().utf8().get_data();
    }
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
