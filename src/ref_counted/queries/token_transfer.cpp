#include "ref_counted/queries/token_transfer.hpp"

Ref<TokenTransferQuery> TokenTransferQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<TokenTransferQuery> TokenTransferQuery::account_addresses(const PackedStringArray& addresses) {
    this->p_account_addresses = addresses;
    return this;
}

Ref<TokenTransferQuery> TokenTransferQuery::token_ids(const PackedStringArray& ids) {
    this->p_token_ids = ids;
    return this;
}

dojo::TokenTransferQuery TokenTransferQuery::get_native() const {
    dojo::TokenTransferQuery query;

    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < p_contract_addresses.size(); i++) {
        ca.push_back(p_contract_addresses[i].utf8().get_data());
    }
    query.contract_addresses = ca;

    std::vector<dojo::FieldElement> aa;
    for (int i = 0; i < p_account_addresses.size(); i++) {
        aa.push_back(p_account_addresses[i].utf8().get_data());
    }
    query.account_addresses = aa;

    std::vector<dojo::U256> tids;
    for (int i = 0; i < p_token_ids.size(); i++) {
        tids.push_back(p_token_ids[i].utf8().get_data());
    }
    query.token_ids = tids;

    auto pagination = get_native_pagination();
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
