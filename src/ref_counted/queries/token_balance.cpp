#include "ref_counted/queries/token_balance.hpp"

Ref<TokenBalanceQuery> TokenBalanceQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<TokenBalanceQuery> TokenBalanceQuery::account_addresses(const PackedStringArray& addresses) {
    this->p_account_addresses = addresses;
    return this;
}

Ref<TokenBalanceQuery> TokenBalanceQuery::token_ids(const PackedStringArray& ids) {
    this->p_token_ids = ids;
    return this;
}

dojo::TokenBalanceQuery TokenBalanceQuery::get_native() const {
    dojo::TokenBalanceQuery query;

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
