//
// Created by hazel on 9/9/25.
//

#ifndef DOJO_CONTRACT_QUERY_H
#define DOJO_CONTRACT_QUERY_H

#include "resources/dojo_query_base.h"
#include "dojo_types.h"
#include "variant/field_element.h"
#include <vector>
 
using namespace godot;
 
class DojoContractQuery : public DojoQueryBase {
    GDCLASS(DojoContractQuery, DojoQueryBase);
 
    TypedArray<String> contract_addresses;
    Array contract_types; // Array of int (ContractType)
 
public:
    DojoContractQuery() {}
    ~DojoContractQuery() {}
 
    void* get_native_query() const override {
        auto* query = new DOJO::ContractQuery();
 
        if (contract_addresses.size() > 0) {
            DOJO::FieldElement* addresses_data = new DOJO::FieldElement[contract_addresses.size()];
            for (int i = 0; i < contract_addresses.size(); ++i) {
                String address_str = contract_addresses[i];
                addresses_data[i] = FieldElement(address_str).get_felt_no_ptr();
            }
            query->contract_addresses.data = addresses_data;
            query->contract_addresses.data_len = contract_addresses.size();
        } else {
            query->contract_addresses.data = nullptr;
            query->contract_addresses.data_len = 0;
        }
 
        if (contract_types.size() > 0) {
            DOJO::ContractType* types_data = new DOJO::ContractType[contract_types.size()];
            for (int i = 0; i < contract_types.size(); ++i) {
                types_data[i] = static_cast<DOJO::ContractType>((int)contract_types[i]);
            }
            query->contract_types.data = types_data;
            query->contract_types.data_len = contract_types.size();
        } else {
            query->contract_types.data = nullptr;
            query->contract_types.data_len = 0;
        }
 
        return query;
    }
 
    TypedArray<String> get_contract_addresses() const { return contract_addresses; }
    void set_contract_addresses(const TypedArray<String>& p_addresses) { contract_addresses = p_addresses; }
 
    Array get_contract_types() const { return contract_types; }
    void set_contract_types(const Array& p_types) { contract_types = p_types; }
 
protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_addresses"), &DojoContractQuery::get_contract_addresses);
        ClassDB::bind_method(D_METHOD("set_contract_addresses", "p_addresses"), &DojoContractQuery::set_contract_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "contract_addresses"), "set_contract_addresses", "get_contract_addresses");
 
        ClassDB::bind_method(D_METHOD("get_contract_types"), &DojoContractQuery::get_contract_types);
        ClassDB::bind_method(D_METHOD("set_contract_types", "p_types"), &DojoContractQuery::set_contract_types);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "contract_types", PROPERTY_HINT_ARRAY_TYPE, "int"), "set_contract_types", "get_contract_types");
    }
};
 
#endif //DOJO_CONTRACT_QUERY_H