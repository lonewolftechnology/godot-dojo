#include "ref_counted/queries/token.hpp"

Ref<TokenQuery> TokenQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<TokenQuery> TokenQuery::token_ids(const PackedStringArray& ids) {
    this->p_token_ids = ids;
    return this;
}

Ref<TokenQuery> TokenQuery::attribute_filter(const String& trait_name, const String& trait_value) {
    AttributeFilter filter;
    filter.trait_name = trait_name;
    filter.trait_value = trait_value;
    p_attribute_filters.push_back(filter);
    return this;
}

dojo::TokenQuery TokenQuery::get_native() const {
    dojo::TokenQuery query;

    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < p_contract_addresses.size(); i++) {
        ca.push_back(p_contract_addresses[i].utf8().get_data());
    }
    query.contract_addresses = ca;

    std::vector<dojo::U256> tids;
    for (int i = 0; i < p_token_ids.size(); i++) {
        tids.push_back(p_token_ids[i].utf8().get_data());
    }
    query.token_ids = tids;

    std::vector<std::shared_ptr<dojo::AttributeFilter>> af;
    for (const auto& filter : p_attribute_filters) {
        auto native_filter = std::make_shared<dojo::AttributeFilter>();
        native_filter->trait_name = filter.trait_name.utf8().get_data();
        native_filter->trait_value = filter.trait_value.utf8().get_data();
        af.push_back(native_filter);
    }
    query.attribute_filters = af;

    auto pagination = get_native_pagination();
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
