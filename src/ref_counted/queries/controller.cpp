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

    auto pagination = get_native_pagination();
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
