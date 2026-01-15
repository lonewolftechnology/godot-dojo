#include "ref_counted/queries/transaction.hpp"

Ref<TransactionQuery> TransactionQuery::transaction_hashes(const PackedStringArray& hashes) {
    this->p_transaction_hashes = hashes;
    return this;
}

Ref<TransactionQuery> TransactionQuery::caller_addresses(const PackedStringArray& addresses) {
    this->p_caller_addresses = addresses;
    return this;
}

Ref<TransactionQuery> TransactionQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<TransactionQuery> TransactionQuery::entrypoints(const PackedStringArray& entrypoints) {
    this->p_entrypoints = entrypoints;
    return this;
}

Ref<TransactionQuery> TransactionQuery::model_selectors(const PackedStringArray& selectors) {
    this->p_model_selectors = selectors;
    return this;
}

Ref<TransactionQuery> TransactionQuery::from_block(const uint64_t& block) {
    this->p_from_block = block;
    return this;
}

Ref<TransactionQuery> TransactionQuery::to_block(const uint64_t& block) {
    this->p_to_block = block;
    return this;
}

dojo::TransactionQuery TransactionQuery::get_native() const {
    dojo::TransactionQuery query;

    // TransactionFilter is optional in TransactionQuery
    // If any filter field is set, we create the filter
    if (!p_transaction_hashes.is_empty() || !p_caller_addresses.is_empty() ||
        !p_contract_addresses.is_empty() || !p_entrypoints.is_empty() ||
        !p_model_selectors.is_empty() || p_from_block > 0 || p_to_block > 0) {

        auto filter = std::make_shared<dojo::TransactionFilter>();

        std::vector<dojo::FieldElement> th;
        for (int i = 0; i < p_transaction_hashes.size(); i++) th.push_back(p_transaction_hashes[i].utf8().get_data());
        filter->transaction_hashes = th;

        std::vector<dojo::FieldElement> ca;
        for (int i = 0; i < p_caller_addresses.size(); i++) ca.push_back(p_caller_addresses[i].utf8().get_data());
        filter->caller_addresses = ca;

        std::vector<dojo::FieldElement> cta;
        for (int i = 0; i < p_contract_addresses.size(); i++) cta.push_back(p_contract_addresses[i].utf8().get_data());
        filter->contract_addresses = cta;

        std::vector<std::string> ep;
        for (int i = 0; i < p_entrypoints.size(); i++) ep.push_back(p_entrypoints[i].utf8().get_data());
        filter->entrypoints = ep;

        std::vector<dojo::FieldElement> ms;
        for (int i = 0; i < p_model_selectors.size(); i++) ms.push_back(p_model_selectors[i].utf8().get_data());
        filter->model_selectors = ms;

        if (p_from_block > 0) filter->from_block = p_from_block;
        if (p_to_block > 0) filter->to_block = p_to_block;

        query.filter = filter;
    }

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
