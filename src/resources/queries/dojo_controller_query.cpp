//
// Created by hazel on 2/6/24.
//

#include "resources/queries/dojo_controller_query.h"

void DojoControllerQuery::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_usernames", "p_usernames"), &DojoControllerQuery::set_usernames);
    ClassDB::bind_method(D_METHOD("get_usernames"), &DojoControllerQuery::get_usernames);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "usernames"), "set_usernames", "get_usernames");

    ClassDB::bind_method(D_METHOD("set_addresses", "p_addresses"), &DojoControllerQuery::set_addresses);
    ClassDB::bind_method(D_METHOD("get_addresses"), &DojoControllerQuery::get_addresses);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "addresses"), "set_addresses", "get_addresses");

    ClassDB::bind_method(D_METHOD("set_limit", "p_limit"), &DojoControllerQuery::set_limit);
    ClassDB::bind_method(D_METHOD("get_limit"), &DojoControllerQuery::get_limit);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "limit"), "set_limit", "get_limit");
}

DojoControllerQuery::DojoControllerQuery() : limit(10) {}

DojoControllerQuery::~DojoControllerQuery() = default;

void DojoControllerQuery::set_usernames(const TypedArray<String>& p_usernames) {
    usernames = p_usernames;
}

TypedArray<String> DojoControllerQuery::get_usernames() const {
    return usernames;
}

void DojoControllerQuery::set_addresses(const TypedArray<String>& p_addresses) {
    addresses = p_addresses;
}

TypedArray<String> DojoControllerQuery::get_addresses() const {
    return addresses;
}

void DojoControllerQuery::set_limit(int p_limit) {
    limit = p_limit;
}

int DojoControllerQuery::get_limit() const {
    return limit;
}
