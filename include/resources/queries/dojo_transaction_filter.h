#ifndef GODOT_DOJO_TRANSACTION_FILTER_H
#define GODOT_DOJO_TRANSACTION_FILTER_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "dojo_types.h"
#include "tools/dojo_helper.h"
#include "variant/field_element.h"

using namespace godot;

class DojoTransactionFilter : public Resource {
    GDCLASS(DojoTransactionFilter, Resource);

    TypedArray<String> transaction_hashes;
    TypedArray<String> caller_addresses;
    TypedArray<String> contract_addresses;
    TypedArray<String> entrypoints;
    TypedArray<String> model_selectors;
    uint64_t from_block = 0;
    uint64_t to_block = 0;

public:
    DojoTransactionFilter() {}
    ~DojoTransactionFilter() {}

    DOJO::TransactionFilter get_native() const {
        DOJO::TransactionFilter filter;

        if (transaction_hashes.size() > 0) {
            auto* data = new DOJO::FieldElement[transaction_hashes.size()];
            for (int i = 0; i < transaction_hashes.size(); ++i) {
                Variant v = transaction_hashes[i];
                String s = v;
                data[i] = FieldElement(s).get_felt_no_ptr();
            }
            filter.transaction_hashes = {data, (uintptr_t)transaction_hashes.size()};
        } else {
            filter.transaction_hashes = {nullptr, 0};
        }

        if (caller_addresses.size() > 0) {
            auto* data = new DOJO::FieldElement[caller_addresses.size()];
            for (int i = 0; i < caller_addresses.size(); ++i) {
                Variant v = caller_addresses[i];
                String s = v;
                data[i] = FieldElement(s).get_felt_no_ptr();
            }
            filter.caller_addresses = {data, (uintptr_t)caller_addresses.size()};
        } else {
            filter.caller_addresses = {nullptr, 0};
        }
        
        if (contract_addresses.size() > 0) {
            auto* data = new DOJO::FieldElement[contract_addresses.size()];
            for (int i = 0; i < contract_addresses.size(); ++i) {
                Variant v = contract_addresses[i];
                String s = v;
                data[i] = FieldElement(s).get_felt_no_ptr();
            }
            filter.contract_addresses = {data, (uintptr_t)contract_addresses.size()};
        } else {
            filter.contract_addresses = {nullptr, 0};
        }

        if (entrypoints.size() > 0) {
            const char** data = new const char*[entrypoints.size()];
            for (int i = 0; i < entrypoints.size(); ++i) {
                Variant v = entrypoints[i];
                String s = v;
                char *c = new char[s.utf8().length() + 1];
                strcpy(c, s.utf8().get_data());
                data[i] = c;
            }
            filter.entrypoints = {data, (uintptr_t)entrypoints.size()};
        } else {
            filter.entrypoints = {nullptr, 0};
        }

        if (model_selectors.size() > 0) {
            auto* data = new DOJO::FieldElement[model_selectors.size()];
            for (int i = 0; i < model_selectors.size(); ++i) {
                Variant v = model_selectors[i];
                String s = v;
                data[i] = FieldElement(s).get_felt_no_ptr();
            }
            filter.model_selectors = {data, (uintptr_t)model_selectors.size()};
        } else {
            filter.model_selectors = {nullptr, 0};
        }

        if (from_block > 0) {
            filter.from_block.tag = DOJO::Someu64;
            filter.from_block.some = from_block;
        } else {
            filter.from_block.tag = DOJO::Noneu64;
        }

        if (to_block > 0) {
            filter.to_block.tag = DOJO::Someu64;
            filter.to_block.some = to_block;
        } else {
            filter.to_block.tag = DOJO::Noneu64;
        }

        return filter;
    }

    void set_transaction_hashes(const TypedArray<String>& p_hashes) { transaction_hashes = p_hashes; }
    TypedArray<String> get_transaction_hashes() const { return transaction_hashes; }
    void set_caller_addresses(const TypedArray<String>& p_addresses) { caller_addresses = p_addresses; }
    TypedArray<String> get_caller_addresses() const { return caller_addresses; }
    void set_contract_addresses(const TypedArray<String>& p_addresses) { contract_addresses = p_addresses; }
    TypedArray<String> get_contract_addresses() const { return contract_addresses; }
    void set_entrypoints(const TypedArray<String>& p_entrypoints) { entrypoints = p_entrypoints; }
    TypedArray<String> get_entrypoints() const { return entrypoints; }
    void set_model_selectors(const TypedArray<String>& p_selectors) { model_selectors = p_selectors; }
    TypedArray<String> get_model_selectors() const { return model_selectors; }
    void set_from_block(uint64_t p_from_block) { from_block = p_from_block; }
    uint64_t get_from_block() const { return from_block; }
    void set_to_block(uint64_t p_to_block) { to_block = p_to_block; }
    uint64_t get_to_block() const { return to_block; }


protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_transaction_hashes"), &DojoTransactionFilter::get_transaction_hashes);
        ClassDB::bind_method(D_METHOD("set_transaction_hashes", "p_hashes"), &DojoTransactionFilter::set_transaction_hashes);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "transaction_hashes"), "set_transaction_hashes", "get_transaction_hashes");

        ClassDB::bind_method(D_METHOD("get_caller_addresses"), &DojoTransactionFilter::get_caller_addresses);
        ClassDB::bind_method(D_METHOD("set_caller_addresses", "p_addresses"), &DojoTransactionFilter::set_caller_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "caller_addresses"), "set_caller_addresses", "get_caller_addresses");

        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &DojoTransactionFilter::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &DojoTransactionFilter::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");

        ClassDB::bind_method(D_METHOD("get_entrypoints"), &DojoTransactionFilter::get_entrypoints);
        ClassDB::bind_method(D_METHOD("set_entrypoints", "p_entrypoints"), &DojoTransactionFilter::set_entrypoints);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "entrypoints"), "set_entrypoints", "get_entrypoints");

        ClassDB::bind_method(D_METHOD("get_model_selectors"), &DojoTransactionFilter::get_model_selectors);
        ClassDB::bind_method(D_METHOD("set_model_selectors", "p_selectors"), &DojoTransactionFilter::set_model_selectors);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "model_selectors"), "set_model_selectors", "get_model_selectors");

        ClassDB::bind_method(D_METHOD("get_from_block"), &DojoTransactionFilter::get_from_block);
        ClassDB::bind_method(D_METHOD("set_from_block", "p_from_block"), &DojoTransactionFilter::set_from_block);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "from_block"), "set_from_block", "get_from_block");

        ClassDB::bind_method(D_METHOD("get_to_block"), &DojoTransactionFilter::get_to_block);
        ClassDB::bind_method(D_METHOD("set_to_block", "p_to_block"), &DojoTransactionFilter::set_to_block);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "to_block"), "set_to_block", "get_to_block");
    }
};

#endif //GODOT_DOJO_TRANSACTION_FILTER_H
