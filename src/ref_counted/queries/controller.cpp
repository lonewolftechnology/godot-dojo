#include "ref_counted/queries/controller.hpp"

Ref<ControllerQuery> ControllerQuery::contract_addresses(const PackedStringArray& addresses) {
    this->p_contract_addresses = addresses;
    return this;
}

Ref<ControllerQuery> ControllerQuery::usernames(const PackedStringArray& usernames) {
    this->p_usernames = usernames;
    return this;
}

dojo::ControllerQuery ControllerQuery::get_native() const {
    dojo::ControllerQuery query;

    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < p_contract_addresses.size(); i++) {
        ca.push_back(p_contract_addresses[i].utf8().get_data());
    }
    query.contract_addresses = ca;

    std::vector<std::string> u;
    for (int i = 0; i < p_usernames.size(); i++) {
        u.push_back(p_usernames[i].utf8().get_data());
    }
    query.usernames = u;

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
