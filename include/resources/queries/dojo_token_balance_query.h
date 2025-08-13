#ifndef DOJO_TOKEN_BALANCE_QUERY_H
#define DOJO_TOKEN_BALANCE_QUERY_H

#include "resources/dojo_query_base.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"

using namespace godot;

class DojoTokenBalanceQuery : public DojoQueryBase {
    GDCLASS(DojoTokenBalanceQuery, DojoQueryBase);

    TypedArray<String> contract_addresses;
    TypedArray<String> account_addresses;
    TypedArray<String> token_ids;

public:
    DojoTokenBalanceQuery() {}
    ~DojoTokenBalanceQuery() {}

    void* get_native_query() const {
        auto* query = new DOJO::TokenBalanceQuery();

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
                addresses_data[i] = DojoHelpers::string_to_field_element(address_str)->get_felt_no_ptr();
            }
            query->contract_addresses.data = addresses_data;
            query->contract_addresses.data_len = contract_addresses.size();
        } else {
            query->contract_addresses.data = nullptr;
            query->contract_addresses.data_len = 0;
        }

        if (account_addresses.size() > 0) {
            DOJO::FieldElement* addresses_data = new DOJO::FieldElement[account_addresses.size()];
            for (int i = 0; i < account_addresses.size(); ++i) {
                String address_str = account_addresses[i];
                addresses_data[i] = DojoHelpers::string_to_field_element(address_str)->get_felt_no_ptr();
            }
            query->account_addresses.data = addresses_data;
            query->account_addresses.data_len = account_addresses.size();
        } else {
            query->account_addresses.data = nullptr;
            query->account_addresses.data_len = 0;
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

    TypedArray<String> get_account_addresses() const { return account_addresses; }
    void set_account_addresses(const TypedArray<String>& p_addresses) { account_addresses = p_addresses; }

    TypedArray<String> get_token_ids() const { return token_ids; }
    void set_token_ids(const TypedArray<String>& p_token_ids) { token_ids = p_token_ids; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &DojoTokenBalanceQuery::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &DojoTokenBalanceQuery::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_account_addresses"), &DojoTokenBalanceQuery::get_account_addresses);
        ClassDB::bind_method(D_METHOD("set_account_addresses", "p_addresses"), &DojoTokenBalanceQuery::set_account_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "account_addresses"), "set_account_addresses", "get_account_addresses");

        ClassDB::bind_method(D_METHOD("get_token_ids"), &DojoTokenBalanceQuery::get_token_ids);
        ClassDB::bind_method(D_METHOD("set_token_ids", "p_token_ids"), &DojoTokenBalanceQuery::set_token_ids);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "token_ids"), "set_token_ids", "get_token_ids");
    }
};

#endif //DOJO_TOKEN_BALANCE_QUERY_H
