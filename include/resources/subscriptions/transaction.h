//
// Created by hazel on 25/08/25.
//

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "resources/dojo_subscription.h"
#include "variant/field_element.h"
#include <vector>

using namespace godot;

class TransactionSubscription : public DojoSubscription
{
    GDCLASS(TransactionSubscription, DojoSubscription);

    bool filter_enabled = false;
    TypedArray<String> transaction_hashes;
    TypedArray<String> caller_addresses;
    TypedArray<String> contract_addresses;
    TypedArray<String> entrypoints;
    TypedArray<String> model_selectors;
    int64_t from_block = -1;
    int64_t to_block = -1;

    // Native data storage
    std::vector<DOJO::FieldElement> native_transaction_hashes;
    std::vector<DOJO::FieldElement> native_caller_addresses;
    std::vector<DOJO::FieldElement> native_contract_addresses;
    std::vector<CharString> native_entrypoints_char_strings;
    std::vector<const char*> native_entrypoints_c_str;
    std::vector<DOJO::FieldElement> native_model_selectors;
    
    DOJO::TransactionFilter native_filter_storage;
    DOJO::COptionTransactionFilter native_coption_filter;

    void update_native_filter() {
        if (!filter_enabled) {
            native_coption_filter.tag = DOJO::COptionTransactionFilter_Tag::NoneTransactionFilter;
            return;
        }

        native_coption_filter.tag = DOJO::COptionTransactionFilter_Tag::SomeTransactionFilter;

        native_filter_storage.transaction_hashes = {native_transaction_hashes.data(), native_transaction_hashes.size()};
        native_filter_storage.caller_addresses = {native_caller_addresses.data(), native_caller_addresses.size()};
        native_filter_storage.contract_addresses = {native_contract_addresses.data(), native_contract_addresses.size()};
        native_filter_storage.entrypoints = {native_entrypoints_c_str.data(), native_entrypoints_c_str.size()};
        native_filter_storage.model_selectors = {native_model_selectors.data(), native_model_selectors.size()};

        if (from_block >= 0) {
            native_filter_storage.from_block.tag = DOJO::COptionu64_Tag::Someu64;
            native_filter_storage.from_block.some = from_block;
        } else {
            native_filter_storage.from_block.tag = DOJO::COptionu64_Tag::Noneu64;
        }

        if (to_block >= 0) {
            native_filter_storage.to_block.tag = DOJO::COptionu64_Tag::Someu64;
            native_filter_storage.to_block.some = to_block;
        } else {
            native_filter_storage.to_block.tag = DOJO::COptionu64_Tag::Noneu64;
        }

        native_coption_filter.some = native_filter_storage;
    }

public:
    Type get_type() const override {
        return TRANSACTION;
    }

    bool is_filter_enabled() const { return filter_enabled; }
    void set_filter_enabled(bool p_enabled) { 
        filter_enabled = p_enabled;
    }

    TypedArray<String> get_transaction_hashes() const { return transaction_hashes; }
    void set_transaction_hashes(const TypedArray<String>& p_hashes) { 
        transaction_hashes = p_hashes;
        native_transaction_hashes.clear();
        native_transaction_hashes.reserve(p_hashes.size());
        for(int i = 0; i < p_hashes.size(); ++i) {
            String hash_str = p_hashes[i];
            native_transaction_hashes.push_back(FieldElement(hash_str, 32).get_felt_no_ptr());
        }
    }

    TypedArray<String> get_caller_addresses() const { return caller_addresses; }
    void set_caller_addresses(const TypedArray<String>& p_addresses) { 
        caller_addresses = p_addresses;
        native_caller_addresses.clear();
        native_caller_addresses.reserve(p_addresses.size());
        for(int i = 0; i < p_addresses.size(); ++i) {
            String address_str = p_addresses[i];
            native_caller_addresses.push_back(FieldElement(address_str, 32).get_felt_no_ptr());
        }
    }

    TypedArray<String> get_contract_addresses() const { return contract_addresses; }
    void set_contract_addresses(const TypedArray<String>& p_addresses) { 
        contract_addresses = p_addresses;
        native_contract_addresses.clear();
        native_contract_addresses.reserve(p_addresses.size());
        for(int i = 0; i < p_addresses.size(); ++i) {
            String address_str = p_addresses[i];
            native_contract_addresses.push_back(FieldElement(address_str, 32).get_felt_no_ptr());
        }
    }

    TypedArray<String> get_entrypoints() const { return entrypoints; }
    void set_entrypoints(const TypedArray<String>& p_entrypoints) { 
        entrypoints = p_entrypoints;
        native_entrypoints_char_strings.clear();
        native_entrypoints_char_strings.reserve(p_entrypoints.size());
        for (int i = 0; i < p_entrypoints.size(); ++i) {
            native_entrypoints_char_strings.push_back(String(p_entrypoints[i]).utf8());
        }

        native_entrypoints_c_str.clear();
        native_entrypoints_c_str.reserve(native_entrypoints_char_strings.size());
        for (const auto& cs : native_entrypoints_char_strings) {
            native_entrypoints_c_str.push_back(cs.get_data());
        }
    }

    TypedArray<String> get_model_selectors() const { return model_selectors; }
    void set_model_selectors(const TypedArray<String>& p_selectors) { 
        model_selectors = p_selectors;
        native_model_selectors.clear();
        native_model_selectors.reserve(p_selectors.size());
        for(int i = 0; i < p_selectors.size(); ++i) {
            String selector_str = p_selectors[i];
            native_model_selectors.push_back(FieldElement(selector_str, 32).get_felt_no_ptr());
        }
    }

    int64_t get_from_block() const { return from_block; }
    void set_from_block(int64_t p_block) { 
        from_block = p_block;
    }

    int64_t get_to_block() const { return to_block; }
    void set_to_block(int64_t p_block) { 
        to_block = p_block;
    }

    DOJO::COptionTransactionFilter get_native_filter() {
        update_native_filter();
        return native_coption_filter;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("is_filter_enabled"), &TransactionSubscription::is_filter_enabled);
        ClassDB::bind_method(D_METHOD("set_filter_enabled", "p_enabled"), &TransactionSubscription::set_filter_enabled);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "filter_enabled"), "set_filter_enabled", "is_filter_enabled");

        ClassDB::bind_method(D_METHOD("get_transaction_hashes"), &TransactionSubscription::get_transaction_hashes);
        ClassDB::bind_method(D_METHOD("set_transaction_hashes", "p_hashes"), &TransactionSubscription::set_transaction_hashes);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "transaction_hashes"), "set_transaction_hashes", "get_transaction_hashes");

        ClassDB::bind_method(D_METHOD("get_caller_addresses"), &TransactionSubscription::get_caller_addresses);
        ClassDB::bind_method(D_METHOD("set_caller_addresses", "p_addresses"), &TransactionSubscription::set_caller_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "caller_addresses"), "set_caller_addresses", "get_caller_addresses");

        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &TransactionSubscription::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &TransactionSubscription::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_entrypoints"), &TransactionSubscription::get_entrypoints);
        ClassDB::bind_method(D_METHOD("set_entrypoints", "p_entrypoints"), &TransactionSubscription::set_entrypoints);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "entrypoints"), "set_entrypoints", "get_entrypoints");

        ClassDB::bind_method(D_METHOD("get_model_selectors"), &TransactionSubscription::get_model_selectors);
        ClassDB::bind_method(D_METHOD("set_model_selectors", "p_selectors"), &TransactionSubscription::set_model_selectors);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "model_selectors"), "set_model_selectors", "get_model_selectors");

        ClassDB::bind_method(D_METHOD("get_from_block"), &TransactionSubscription::get_from_block);
        ClassDB::bind_method(D_METHOD("set_from_block", "p_block"), &TransactionSubscription::set_from_block);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "from_block"), "set_from_block", "get_from_block");

        ClassDB::bind_method(D_METHOD("get_to_block"), &TransactionSubscription::get_to_block);
        ClassDB::bind_method(D_METHOD("set_to_block", "p_block"), &TransactionSubscription::set_to_block);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "to_block"), "set_to_block", "get_to_block");
    }
};

#endif //TRANSACTION_H
