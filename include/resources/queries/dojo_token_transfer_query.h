//
// Created by chozo on 9/29/25.
//

#ifndef DOJO_TOKEN_TRANSFER_QUERY_H
#define DOJO_TOKEN_TRANSFER_QUERY_H
#include "resources/dojo_query_base.h"
#include "ref_counted/options/option_u32.h"
#include "ref_counted/options/option_char.h"
#include "variant/ty/dojo_array.h"

using namespace godot;

class DojoTokenTransferQuery : public DojoQueryBase {
    GDCLASS(DojoTokenTransferQuery, DojoQueryBase);

    TypedArray<String> contract_addresses;
    TypedArray<String> account_addresses;
    TypedArray<String> token_ids;

public:
    DojoTokenTransferQuery() {}
    ~DojoTokenTransferQuery() {}

    void set_contract_addresses(const TypedArray<String>& p_contract_addresses) { contract_addresses = p_contract_addresses; }
    TypedArray<String> get_contract_addresses() const { return contract_addresses; }

    void set_account_addresses(const TypedArray<String>& p_account_addresses) { account_addresses = p_account_addresses; }
    TypedArray<String> get_account_addresses() const { return account_addresses; }

    void set_token_ids(const TypedArray<String>& p_token_ids) { token_ids = p_token_ids; }
    TypedArray<String> get_token_ids() const { return token_ids; }

    void* get_native_query() const {
        auto* query = new DOJO::TokenTransferQuery();

        // Pagination
        query->pagination.direction = get_direction();

        OptionU32 limit_option(get_limit());
        query->pagination.limit = limit_option.get_native_option();

        if (!get_cursor().is_empty()) {
            query->pagination.cursor.tag = DOJO::Somec_char;
            char *c_str_cursor = new char[get_cursor().utf8().length() + 1];
            strcpy(c_str_cursor, get_cursor().utf8().get_data());
            query->pagination.cursor.some = c_str_cursor;
        } else {
            query->pagination.cursor.tag = DOJO::Nonec_char;
        }

        // Order By
        if (get_order_by().size() > 0) {
            auto* native_order_by_arr = new DOJO::OrderBy[get_order_by().size()];
            for (int i = 0; i < get_order_by().size(); ++i) {
                Ref<DojoOrderBy> order_by_ref = get_order_by()[i];
                if (order_by_ref.is_valid()) {
                    char *field_str = new char[order_by_ref->get_field().utf8().length() + 1];
                    strcpy(field_str, order_by_ref->get_field().utf8().get_data());

                    native_order_by_arr[i] = {
                        field_str,
                        order_by_ref->get_direction()
                    };
                }
            }
            query->pagination.order_by = {native_order_by_arr, (uintptr_t)get_order_by().size()};
        } else {
            query->pagination.order_by = {nullptr, 0};
        }

        // contract_addresses
        query->contract_addresses = DojoArrayHelpers::string_array_to_native_carray_felt(contract_addresses);

        // account_addresses
        query->account_addresses = DojoArrayHelpers::string_array_to_native_carray_felt(account_addresses);

        // token_ids
        query->token_ids = DojoArrayHelpers::string_array_to_native_carray_u256(token_ids);

        return query;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &DojoTokenTransferQuery::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_contract_addresses"), &DojoTokenTransferQuery::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_account_addresses"), &DojoTokenTransferQuery::get_account_addresses);
        ClassDB::bind_method(D_METHOD("set_account_addresses", "p_account_addresses"), &DojoTokenTransferQuery::set_account_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "account_addresses"), "set_account_addresses", "get_account_addresses");

        ClassDB::bind_method(D_METHOD("get_token_ids"), &DojoTokenTransferQuery::get_token_ids);
        ClassDB::bind_method(D_METHOD("set_token_ids", "p_token_ids"), &DojoTokenTransferQuery::set_token_ids);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "token_ids"), "set_token_ids", "get_token_ids");
    }
};

#endif //DOJO_TOKEN_TRANSFER_QUERY_H