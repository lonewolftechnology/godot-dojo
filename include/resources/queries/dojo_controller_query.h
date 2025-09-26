#ifndef GODOT_DOJO_CONTROLLER_QUERY_H
#define GODOT_DOJO_CONTROLLER_QUERY_H

#include "resources/dojo_query_base.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"
#include "variant/field_element.h"

using namespace godot;

class DojoControllerQuery : public DojoQueryBase {
    GDCLASS(DojoControllerQuery, DojoQueryBase);

    TypedArray<String> contract_addresses;
    TypedArray<String> usernames;

public:
    DojoControllerQuery() {}
    ~DojoControllerQuery() {}

    void* get_native_query() const {
        auto* query = new DOJO::ControllerQuery();
        
        // Setup pagination
        query->pagination.direction = get_direction();
        
        if (get_limit() > 0) {
            query->pagination.limit.tag = DOJO::Someu32;
            query->pagination.limit.some = get_limit();
        } else {
            query->pagination.limit.tag = DOJO::Noneu32;
        }

        if (!get_cursor().is_empty()) {
            query->pagination.cursor.tag = DOJO::Somec_char;
            char *c_str_cursor = new char[get_cursor().utf8().length() + 1];
            strcpy(c_str_cursor, get_cursor().utf8().get_data());
            query->pagination.cursor.some = c_str_cursor;
        } else {
            query->pagination.cursor.tag = DOJO::Nonec_char;
        }
        
        query->pagination.order_by.data = nullptr;
        query->pagination.order_by.data_len = 0;

        if (contract_addresses.size() > 0) {
            DOJO::FieldElement* addresses_data = new DOJO::FieldElement[contract_addresses.size()];
            for (int i = 0; i < contract_addresses.size(); ++i) {
                String address_str = contract_addresses[i];
                addresses_data[i] = FieldElement::from_string(address_str);
            }
            query->contract_addresses.data = addresses_data;
            query->contract_addresses.data_len = contract_addresses.size();
        } else {
            query->contract_addresses.data = nullptr;
            query->contract_addresses.data_len = 0;
        }

        if (usernames.size() > 0) {
            const char** usernames_data = new const char*[usernames.size()];
            for (int i = 0; i < usernames.size(); ++i) {
                String username_str = usernames[i];
                char *c_str = new char[username_str.utf8().length() + 1];
                strcpy(c_str, username_str.utf8().get_data());
                usernames_data[i] = c_str;
            }
            query->usernames.data = usernames_data;
            query->usernames.data_len = usernames.size();
        } else {
            query->usernames.data = nullptr;
            query->usernames.data_len = 0;
        }

        return query;
    }

    TypedArray<String> get_contract_addresses() const { return contract_addresses; }
    void set_contract_addresses(const TypedArray<String>& p_addresses) { contract_addresses = p_addresses; }

    TypedArray<String> get_usernames() const { return usernames; }
    void set_usernames(const TypedArray<String>& p_usernames) { usernames = p_usernames; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &DojoControllerQuery::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &DojoControllerQuery::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_usernames"), &DojoControllerQuery::get_usernames);
        ClassDB::bind_method(D_METHOD("set_usernames", "p_usernames"), &DojoControllerQuery::set_usernames);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "usernames"), "set_usernames", "get_usernames");
    }
};

#endif //GODOT_DOJO_CONTROLLER_QUERY_H
