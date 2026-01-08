//
// Created by hazel on 25/08/25.
//

#ifndef STARKNET_H
#define STARKNET_H

#include "resources/dojo_subscription.h"
#include "ref_counted/options/option_field_element.h"

using namespace godot;

class StarknetSubscription : public DojoSubscription
{
    GDCLASS(StarknetSubscription, DojoSubscription);

    Array keys; // Array of Ref<DojoOptionFieldElement>
    DOJO::PatternMatching pattern_matching = DOJO::PatternMatching::FixedLen;
    TypedArray<String> models;

    // Native data storage
    mutable std::vector<DOJO::COptionFieldElement> native_keys;
    mutable std::vector<CharString> native_models_char_strings;
    mutable std::vector<const char*> native_models_c_str;
    mutable DOJO::KeysClause native_clause_storage;

    void update_native_clause() const {
        native_keys.clear();
        native_keys.reserve(keys.size());
        for (int i = 0; i < keys.size(); ++i) {
            Ref<DojoOptionFieldElement> key = keys[i];
            if (key.is_valid()) {
                native_keys.push_back(key->get_native_option());
            } else {
                native_keys.push_back({DOJO::COptionFieldElement_Tag::NoneFieldElement, {}});
            }
        }

        native_models_char_strings.clear();
        native_models_char_strings.reserve(models.size());
        for (int i = 0; i < models.size(); ++i) {
            native_models_char_strings.push_back(static_cast<String>(models[i]).utf8());
        }

        native_models_c_str.clear();
        native_models_c_str.reserve(native_models_char_strings.size());
        for (const auto& cs : native_models_char_strings) {
            native_models_c_str.push_back(cs.get_data());
        }

        native_clause_storage.keys = {native_keys.data(), native_keys.size()};
        native_clause_storage.pattern_matching = pattern_matching;
        native_clause_storage.models = {native_models_c_str.data(), native_models_c_str.size()};
    }

public:
    Type get_type() const override {
        return STARKNET;
    }

    Array get_keys() const { return keys; }
    void set_keys(const Array& p_keys) {
        keys = p_keys;
    }

    DOJO::PatternMatching get_pattern_matching() const { return pattern_matching; }
    void set_pattern_matching(DOJO::PatternMatching p_pm) { pattern_matching = p_pm; }

    TypedArray<String> get_models() const { return models; }
    void set_models(const TypedArray<String>& p_models) {
        models = p_models;
    }

    const DOJO::KeysClause* get_native_clause() const {
        update_native_clause();
        return &native_clause_storage;
    }

protected:
    static void _bind_methods() {
        // Properties
        ClassDB::bind_method(D_METHOD("get_keys"), &StarknetSubscription::get_keys);
        ClassDB::bind_method(D_METHOD("set_keys", "p_keys"), &StarknetSubscription::set_keys);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "keys", PROPERTY_HINT_ARRAY_TYPE, vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "DojoOptionFieldElement")), "set_keys", "get_keys");

        ClassDB::bind_method(D_METHOD("get_pattern_matching"), &StarknetSubscription::get_pattern_matching);
        ClassDB::bind_method(D_METHOD("set_pattern_matching", "p_pm"), &StarknetSubscription::set_pattern_matching);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "pattern_matching", PROPERTY_HINT_ENUM, "FixedLen,VariableLen"), "set_pattern_matching", "get_pattern_matching");

        ClassDB::bind_method(D_METHOD("get_models"), &StarknetSubscription::get_models);
        ClassDB::bind_method(D_METHOD("set_models", "p_models"), &StarknetSubscription::set_models);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "models"), "set_models", "get_models");

        BIND_ENUM_CONSTANT(DOJO::PatternMatching::FixedLen);
        BIND_ENUM_CONSTANT(DOJO::PatternMatching::VariableLen);
    }
};


#endif //STARKNET_H
