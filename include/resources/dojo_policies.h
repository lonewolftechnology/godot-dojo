//
// Created by hazel on 21/06/25.
//

#ifndef DOJO_POLICIES_H
#define DOJO_POLICIES_H

#include "tools/logger.h"
#include "godot_cpp/classes/resource.hpp"
#include "variant/field_element.h"
#include "dojo_policy.h"

using namespace godot;

class DojoPolicies : public Resource
{
    GDCLASS(DojoPolicies, Resource);

public:
    DojoPolicies()
    {
    };

    ~DojoPolicies()
    {
    };

    void set_dojo_contract(const String& p_contract)
    {
        dojo_contract = p_contract;
        emit_changed();
    };
    String get_dojo_contract() const { return dojo_contract; };

    void set_policies(const Dictionary& p_policies)
    {
        for (int i; i < p_policies.size(); ++i)
        {
            Ref<DojoPolicy> _policy = {};
            _policy.instantiate();
            String _key = p_policies.keys()[i];
            _policy->set_method(_key);
            _policy->set_description(p_policies[_key]);
            policies.push_back(_policy);
        }
        emit_changed();
    };
    TypedArray<DojoPolicy> get_policies() { return policies; };

    String get_name() const { return name; }

    void set_name(const String& p_name)
    {
        name = p_name;
        emit_changed();
    }

    bool is_empty() const { return policies.is_empty(); }

    void add_policy(const Variant& p_policy)
    {
        if (p_policy.get_type() == Variant::DICTIONARY)
        {
            // TODO: Refactor, from_dictionary is for fully policies, not for individual
            Ref<DojoPolicy> new_policy = DojoPolicy::from_dictionary(p_policy);
            if (new_policy.is_valid())
            {
                policies.push_back(new_policy);
                emit_changed();
            }
            else
            {
                Logger::error("DojoPolicies: Failed to create DojoPolicy from the provided Dictionary.");
            }
        }
        else if (p_policy.get_type() == Variant::OBJECT)
        {
            // Safely cast the object from the variant to ensure it's the correct type.
            Ref<RefCounted> obj = p_policy;

            if (obj.is_valid() && obj->is_class(DojoPolicy::get_class_static()))
            {
                // The cast was successful, it's a valid DojoPolicy.
                // We can add the original variant, which will be correctly handled by TypedArray.
                policies.push_back(p_policy);
                emit_changed();
            }
            else
            {
                // The object was either null or not a DojoPolicy.
                Logger::error("DojoPolicies: The provided Object is not a valid DojoPolicy resource.");
            }
        }
        else if (p_policy.get_type() != Variant::NIL)
        {
            Logger::error("DojoPolicies: add_policy expects a Dictionary or a DojoPolicy resource.");
        }
    }

    std::vector<DOJO::Policy> build() const
    {
        if (dojo_contract.is_empty() || policies.is_empty())
        {
            return {};
        }

        DOJO::FieldElement contract = FieldElement::from_string(dojo_contract);
        std::vector<DOJO::Policy> result;
        result.reserve(policies.size());

        for (int i = 0; i < policies.size(); i++)
        {
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

    Dictionary to_dictionary() const
    {
        Dictionary dict;
        dict["name"] = name;
        dict["contract"] = dojo_contract;

        Array policies_array;
        for (int i = 0; i < policies.size(); ++i)
        {
            Ref<DojoPolicy> policy = policies[i];
            if (policy.is_valid())
            {
                policies_array.push_back(policy->to_dictionary());
            }
        }
        dict["policies"] = policies_array;

        return dict;
    }

    static Ref<DojoPolicies> from_dictionary(const Dictionary& dict)
    {
        Ref<DojoPolicies> policies_res;
        policies_res.instantiate();
        policies_res->_from_dictionary(dict);
        return policies_res;
    }

protected:
    void _from_dictionary(const Dictionary& dict)
    {
        if (dict.has("name"))
        {
            set_name(dict["name"]);
        }
        if (dict.has("contract"))
        {
            set_dojo_contract(dict["contract"]);
        }

        if (dict.has("policies") && dict["policies"].get_type() == Variant::ARRAY)
        {
            Array policies_array = dict["policies"];
            TypedArray<DojoPolicy> new_policies;
            for (int i = 0; i < policies_array.size(); ++i)
            {
                Ref<DojoPolicy> new_policy = DojoPolicy::from_dictionary(policies_array[i]);
                new_policies.push_back(new_policy);
            }
            // set_policies(new_policies);
        }
    }

    String name;
    String dojo_contract;
    TypedArray<DojoPolicy> policies;

    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_name"), &DojoPolicies::get_name);
        ClassDB::bind_method(D_METHOD("set_name", "name"), &DojoPolicies::set_name);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

        ClassDB::bind_method(D_METHOD("set_dojo_contract", "contract"), &DojoPolicies::set_dojo_contract);
        ClassDB::bind_method(D_METHOD("get_dojo_contract"), &DojoPolicies::get_dojo_contract);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "contract"), "set_dojo_contract", "get_dojo_contract");

        ClassDB::bind_method(D_METHOD("set_policies", "policies"), &DojoPolicies::set_policies);
        ClassDB::bind_method(D_METHOD("get_policies"), &DojoPolicies::get_policies);
        ADD_PROPERTY(
            PropertyInfo(Variant::ARRAY, "policies", PROPERTY_HINT_TYPE_STRING, String::num(Variant::OBJECT) + "/" +
                String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":DojoPolicy"), "set_policies", "get_policies");

        ClassDB::bind_method(D_METHOD("to_dictionary"), &DojoPolicies::to_dictionary);
        ClassDB::bind_static_method(get_class_static(), D_METHOD("from_dictionary", "dictionary"),
                                    &DojoPolicies::from_dictionary);

        ClassDB::bind_method(D_METHOD("add_policy", "policy"), &DojoPolicies::add_policy);
    };
};
#endif //DOJO_POLICIES_H
