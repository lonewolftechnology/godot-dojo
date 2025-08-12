#ifndef DOJO_TOKEN_QUERY_H
#define DOJO_TOKEN_QUERY_H

#include "resources/dojo_query_base.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"

using namespace godot;

class DojoTokenQuery : public DojoQueryBase {
    GDCLASS(DojoTokenQuery, DojoQueryBase);

    TypedArray<String> contract_addresses;
    TypedArray<String> token_ids;

public:
    DojoTokenQuery() {}
    ~DojoTokenQuery() {}

    void* get_native_query() const override {
        auto* query = new DOJO::TokenQuery();
        
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
                DOJO::U256 u256_val = DojoHelpers::string_to_u256(address_str);
                memcpy(addresses_data[i].data, u256_val.data, 32);
            }
            query->contract_addresses.data = addresses_data;
            query->contract_addresses.data_len = contract_addresses.size();
        } else {
            query->contract_addresses.data = nullptr;
            query->contract_addresses.data_len = 0;
        }

        if (token_ids.size() > 0) {
            DOJO::U256* tokens_data = new DOJO::U256[token_ids.size()];
            for (int i = 0; i < token_ids.size(); ++i) {
                String token_id_str = token_ids[i];
                tokens_data[i] = DojoHelpers::string_to_u256(token_id_str);
            }
            query->token_ids.data = tokens_data;
            query->token_ids.data_len = token_ids.size();
        } else {
            query->token_ids.data = nullptr;
            query->token_ids.data_len = 0;
        }

        return query;
    }

    TypedArray<String> get_contract_addresses() const { return contract_addresses; }
    void set_contract_addresses(const TypedArray<String>& p_addresses) { contract_addresses = p_addresses; }

    TypedArray<String> get_token_ids() const { return token_ids; }
    void set_token_ids(const TypedArray<String>& p_token_ids) { token_ids = p_token_ids; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &DojoTokenQuery::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &DojoTokenQuery::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_token_ids"), &DojoTokenQuery::get_token_ids);
        ClassDB::bind_method(D_METHOD("set_token_ids", "p_token_ids"), &DojoTokenQuery::set_token_ids);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "token_ids"), "set_token_ids", "get_token_ids");
    }
};

#endif //DOJO_TOKEN_QUERY_H
