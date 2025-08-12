//
// Created by hazel on 25/08/25.
//

#ifndef TOKEN_H
#define TOKEN_H

#include "resources/dojo_subscription.h"
#include "variant/field_element.h"
#include <vector>

using namespace godot;

class TokenSubscription : public DojoSubscription
{
    GDCLASS(TokenSubscription, DojoSubscription);

    TypedArray<String> contract_addresses;
    TypedArray<String> token_ids;

    // Native data storage
    mutable std::vector<DOJO::FieldElement> native_contract_addresses;
    mutable std::vector<DOJO::U256> native_token_ids;

public:
    Type get_type() const override {
        return TOKEN;
    }

    TypedArray<String> get_contract_addresses() const { return contract_addresses; }
    void set_contract_addresses(const TypedArray<String>& p_addresses) {
        contract_addresses = p_addresses;
        native_contract_addresses.clear();
        native_contract_addresses.reserve(p_addresses.size());
        for (int i = 0; i < p_addresses.size(); ++i) {
            String address_str = p_addresses[i];
            native_contract_addresses.push_back(FieldElement(address_str, 32).get_felt_no_ptr());
        }
    }

    TypedArray<String> get_token_ids() const { return token_ids; }
    void set_token_ids(const TypedArray<String>& p_ids) {
        token_ids = p_ids;
        native_token_ids.clear();
        native_token_ids.reserve(p_ids.size());
        for (int i = 0; i < p_ids.size(); ++i) {
            DOJO::U256 u256_val = {};
            String val_str = p_ids[i];
            if (val_str.is_valid_int()) {
                 u256_val.data[0] = val_str.to_int();
            }
            native_token_ids.push_back(u256_val);
        }
    }

    DOJO::CArrayFieldElement get_native_contract_addresses() const {
        return {native_contract_addresses.data(), native_contract_addresses.size()};
    }

    DOJO::CArrayU256 get_native_token_ids() const {
        return {native_token_ids.data(), native_token_ids.size()};
    }


protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &TokenSubscription::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &TokenSubscription::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_token_ids"), &TokenSubscription::get_token_ids);
        ClassDB::bind_method(D_METHOD("set_token_ids", "p_ids"), &TokenSubscription::set_token_ids);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "token_ids"), "set_token_ids", "get_token_ids");
    }
};

#endif //TOKEN_H
