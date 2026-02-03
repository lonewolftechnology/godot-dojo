#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"

using namespace godot;

class KeysClause : public DojoClause {
    GDCLASS(KeysClause, DojoClause)

    TypedArray<String> p_keys; // Option Felt
    dojo::PatternMatching p_patter_matching;
    TypedArray<String> p_models;

public:
    KeysClause();
    ~KeysClause();

    Ref<KeysClause> add_key(const String& hashed_key);
    Ref<KeysClause> remove_key(const String& hashed_key);
    Ref<KeysClause> keys(const PackedStringArray& keys);
    Ref<KeysClause> add_model(const String& model);
    Ref<KeysClause> remove_model(const String& model);
    Ref<KeysClause> models(const PackedStringArray& models);
    Ref<KeysClause> pattern(const int64_t& pattern);

    dojo::Clause get_native() const override;
    Dictionary to_dict() const override;

protected:
    static void _bind_methods() {
        ClassDB::bind_integer_constant(get_class_static(), "PatternMatching", "FixedLen", (int)dojo::PatternMatching::kFixedLen);
        ClassDB::bind_integer_constant(get_class_static(), "PatternMatching", "VariableLen", (int)dojo::PatternMatching::kVariableLen);

        ClassDB::bind_method(D_METHOD("add_key", "hashed_key"), &KeysClause::add_key);
        ClassDB::bind_method(D_METHOD("remove_key", "hashed_key"), &KeysClause::remove_key);
        ClassDB::bind_method(D_METHOD("keys", "keys"), &KeysClause::keys);
        ClassDB::bind_method(D_METHOD("add_model", "model"), &KeysClause::add_model);
        ClassDB::bind_method(D_METHOD("remove_model", "model"), &KeysClause::remove_model);
        ClassDB::bind_method(D_METHOD("models", "models"), &KeysClause::models);
        ClassDB::bind_method(D_METHOD("pattern", "pattern"), &KeysClause::pattern);
        ClassDB::bind_method(D_METHOD("to_dict"), &KeysClause::to_dict);
    }
};

// VARIANT_ENUM_CAST(dojo::PatternMatching)