#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "variant/ty/dojo_array.h"

using namespace godot;

class KeysClause : public RefCounted {
    GDCLASS(KeysClause, RefCounted)

    TypedArray<String> p_keys; // Option Felt
    DOJO::PatternMatching p_patter_matching;
    TypedArray<String> p_models;

public:
    KeysClause();
    ~KeysClause();

    Ref<KeysClause> add_key(const String& hashed_key) {
        this->p_keys.push_back(hashed_key);
        return this;
    }

    Ref<KeysClause> remove_key(const String& hashed_key) {
        this->p_keys.erase(hashed_key);
        return this;
    }

    Ref<KeysClause> keys(const PackedStringArray& keys) {
        this->p_keys = keys;
        return this;
    }

    Ref<KeysClause> add_model(const String& model) {
        this->p_models.push_back(model);
        return this;
    }

    Ref<KeysClause> remove_model(const String& model) {
        this->p_models.erase(model);
        return this;
    }

    Ref<KeysClause> models(const PackedStringArray& models) {
        this->p_models = models;
        return this;
    }

    Ref<KeysClause> pattern(const int64_t& pattern) {
        this->p_patter_matching = static_cast<DOJO::PatternMatching>(pattern);
        return this;
    }

    DOJO::KeysClause get_native() {
        DOJO::KeysClause clause = {};
        clause.keys = DojoArrayHelpers::option_field_element_array_to_native_carray(p_keys);
        clause.pattern_matching = p_patter_matching;
        clause.models = DojoArrayHelpers::string_array_to_native_carray_str(p_models);
        return clause;
    }

    Dictionary to_dict() {
        Dictionary result = {};
        result["keys"] = p_keys;
        result["pattern"] = p_patter_matching;
        result["models"] = p_models;
        return result;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_integer_constant(get_class_static(), "PatternMatching", "FixedLen", DOJO::PatternMatching::FixedLen);
        ClassDB::bind_integer_constant(get_class_static(), "PatternMatching", "VariableLen", DOJO::PatternMatching::VariableLen);

    }
};

VARIANT_ENUM_CAST(DOJO::PatternMatching)