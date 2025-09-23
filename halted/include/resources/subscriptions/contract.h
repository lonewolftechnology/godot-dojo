//
// Created by hazel on 9/9/25.
//
 
#ifndef CONTRACT_H
#define CONTRACT_H
 
#include "resources/dojo_subscription.h"
#include "variant/field_element.h"
 
using namespace godot;
 
class ContractSubscription : public DojoSubscription {
    GDCLASS(ContractSubscription, DojoSubscription);
 
    String contract_address;
    mutable DOJO::FieldElement native_address_storage;
 
public:
    Type get_type() const override {
        return CONTRACT;
    }
 
    String get_contract_address() const { return contract_address; }
    void set_contract_address(const String& p_address) {
        contract_address = p_address;
    }
 
    const DOJO::FieldElement* get_native_contract_address() const {
        if (contract_address.is_empty()) {
            return nullptr;
        }
        native_address_storage = FieldElement(contract_address).get_felt_no_ptr();
        return &native_address_storage;
    }
 
protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_address"), &ContractSubscription::get_contract_address);
        ClassDB::bind_method(D_METHOD("set_contract_address", "p_address"), &ContractSubscription::set_contract_address);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "contract_address"), "set_contract_address", "get_contract_address");
    }
};
 
#endif //CONTRACT_H