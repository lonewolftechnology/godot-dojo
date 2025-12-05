#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "variant/ty/dojo_array.h"

using namespace godot;

class HashedKeysClause : public RefCounted {
    GDCLASS(HashedKeysClause, RefCounted)

    TypedArray<String> p_hashed_keys;

public:
    HashedKeysClause();
    ~HashedKeysClause();

    Ref<HashedKeysClause> add_key(const String& hashed_key) {
        this->p_hashed_keys.push_back(hashed_key);
        return this;
    }

    Ref<HashedKeysClause> remove_key(const String& hashed_key) {
        this->p_hashed_keys.erase(hashed_key);
        return this;
    }

    Ref<HashedKeysClause> keys(const PackedStringArray& keys) {
        this->p_hashed_keys = keys;
        return this;
    }

    DOJO::CArrayFieldElement get_native() const {
        return DojoArrayHelpers::string_array_to_native_carray_felt(p_hashed_keys);
    }

    Dictionary to_dict() const {
        Dictionary result = {};
        result["hashed_keys"] = p_hashed_keys;
        return result;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("add_key", "hashed_key"), &HashedKeysClause::add_key);
        ClassDB::bind_method(D_METHOD("remove_key", "hashed_key"), &HashedKeysClause::remove_key);
        ClassDB::bind_method(D_METHOD("keys", "hashed_keys"), &HashedKeysClause::keys);
        ClassDB::bind_method(D_METHOD("to_dict"), &HashedKeysClause::to_dict);
    }
};