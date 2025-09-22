//
// Created by hazel on 25/08/25.
//

#ifndef INDEXER_H
#define INDEXER_H

#include "resources/dojo_subscription.h"
#include "variant/field_element.h"

using namespace godot;

class IndexerSubscription : public DojoSubscription
{
    GDCLASS(IndexerSubscription, DojoSubscription);

    Ref<FieldElement> contract_address;
    mutable DOJO::FieldElement native_contract_address; // Store the native struct
    mutable bool has_contract_address = false;

public:
    Type get_type() const override {
        return INDEXER;
    }

    Ref<FieldElement> get_contract_address() const {
        return contract_address;
    }

    void set_contract_address(const Ref<FieldElement>& p_address) {
        contract_address = p_address;
    }

    DOJO::FieldElement* get_native_contract_address() const {
        if (contract_address.is_valid()) {
            native_contract_address = contract_address->get_felt_no_ptr(); // Copy the value
            has_contract_address = true;
            return &native_contract_address; // Return a stable pointer
        }
        has_contract_address = false;
        return nullptr;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_contract_address"), &IndexerSubscription::get_contract_address);
        ClassDB::bind_method(D_METHOD("set_contract_address", "p_address"), &IndexerSubscription::set_contract_address);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "contract_address", PROPERTY_HINT_RESOURCE_TYPE, "FieldElement"), "set_contract_address", "get_contract_address");
    }
};

#endif //INDEXER_H
