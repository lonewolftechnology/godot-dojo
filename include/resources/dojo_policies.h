//
// Created by hazel on 21/06/25.
//

#ifndef DOJO_POLICIES_H
#define DOJO_POLICIES_H

#include "debug_macros.h"
#include "godot_cpp/classes/resource.hpp"
#include "variant/field_element.h"
#include "dojo_policy.h"

using namespace godot;

class DojoPolicies : public Resource
{
    GDCLASS(DojoPolicies, Resource);

public:
    DojoPolicies(){};

    ~DojoPolicies(){};

    void set_dojo_contract(const String& p_contract)
    {
        dojo_contract = p_contract;
        emit_changed();
    };
    String get_dojo_contract() const { return dojo_contract; };

    void set_policies(const TypedArray<DojoPolicy>& p_policies)
    {
        policies = p_policies;
        emit_changed();
    };
    TypedArray<DojoPolicy> get_policies() { return policies; };
    bool is_empty() const { return policies.is_empty(); }

    std::vector<DOJO::Policy> build() const {
    if (dojo_contract.is_empty() || policies.is_empty()) {
        return {};
    }

    DOJO::FieldElement contract = FieldElement::from_string(dojo_contract);
    std::vector<DOJO::Policy> result;
    result.reserve(policies.size());

    for (int i = 0; i < policies.size(); i++) {
        Ref<DojoPolicy> policy = policies[i];
        if (!policy.is_valid()) continue;

        DOJO::Policy dojo_policy = {};
        dojo_policy.target = contract;
        dojo_policy.method = policy->get_method_ctr();
        dojo_policy.description = policy->get_description_ctr();
        
        result.push_back(dojo_policy);
    }

    return result;
}

protected:
    String dojo_contract;
    TypedArray<DojoPolicy> policies;

    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_dojo_contract", "contract"), &DojoPolicies::set_dojo_contract);
        ClassDB::bind_method(D_METHOD("get_dojo_contract"), &DojoPolicies::get_dojo_contract);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "contract"), "set_dojo_contract", "get_dojo_contract");

        ClassDB::bind_method(D_METHOD("set_policies", "policies"), &DojoPolicies::set_policies);
        ClassDB::bind_method(D_METHOD("get_policies"), &DojoPolicies::get_policies);
        ADD_PROPERTY(
            PropertyInfo(Variant::ARRAY, "policies", PROPERTY_HINT_TYPE_STRING, String::num(Variant::OBJECT) + "/" +
                String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":DojoPolicy"), "set_policies", "get_policies");
    };
};
#endif //DOJO_POLICIES_H